#include "WebManager.h"
#include "Application.h"
#include "DebugConsole.h"
#include "External/miniz.h"
#include <filesystem>
#include <fstream>
#include <chrono>

#include "Objects/Event.h"

static Application* a = nullptr;
static DebugConsole* d = nullptr;

bool WebManager::Init(Application* owner) {
	a = owner;
	d = owner->GetDebugConsole();
	return true;
}

void WebManager::Update() {
	std::vector<WebEvent> events;
	drain(events);

	// Call Lime.Web events
	for (auto& e : events) {
		switch (e.type) {
			case WebEventType::GetComplete: {
				// Lime.Web.onGetComplete
				LimeOnGetComplete.get()->engineRun([&](const std::string& msg) { d->PostError(msg); }, e.code, e.data);
				break;
			}
			case WebEventType::GetFail: {
				// Lime.Web.onGetFail
				LimeOnGetFail.get()->engineRun([&](const std::string& msg) { d->PostError(msg); }, e.code);
				break;
			}
			case WebEventType::DownloadComplete: {
				// Lime.Web.onDownloadComplete
				LimeOnDownloadComplete.get()->engineRun([&](const std::string& msg) { d->PostError(msg); }, e.code, e.data);
				break;
			}
			case WebEventType::DownloadFail: {
				// Lime.Web.onDownloadFail
				LimeOnDownloadFail.get()->engineRun([&](const std::string& msg) { d->PostError(msg); }, e.code);
				break;
			}
		}
	}
}

void WebManager::push(WebEvent e) {
	std::lock_guard lock(mu);
	queue.push_back(std::move(e));
}

void WebManager::drain(std::vector<WebEvent>& out) {
	std::lock_guard lock(mu);
	out.clear();
	out.swap(queue);
}

std::unique_ptr<httplib::Client> WebManager::makeClient(const std::string& url, std::string& outPath){
	auto sep = url.find("://");
	if (sep == std::string::npos) return nullptr;
	std::string scheme = url.substr(0, sep);
	std::string rest = url.substr(sep + 3);
	auto slash = rest.find('/');
	std::string host = slash == std::string::npos ? rest : rest.substr(0, slash);
	outPath = slash == std::string::npos ? "/" : rest.substr(slash);

	if (host.empty()) return nullptr;

	auto cli = std::make_unique<httplib::Client>((scheme + "://" + host).c_str());
	cli->set_follow_location(true);

	int sec = timeOutMs / 1000;
	int usec = (timeOutMs % 1000) * 1000;
	cli->set_connection_timeout(sec, usec);
	cli->set_read_timeout(sec, usec);
	cli->set_write_timeout(sec, usec);

	return cli;
}

void WebManager::get(const std::string& url) {
	std::thread([this, url] {
		std::string path;
		auto cli = makeClient(url, path);
		if (!cli) {
			push({ WebEventType::GetFail, -1, {} });
			return;
		}

		std::string body;
		auto res = cli->Get(path, [&](const char* data, size_t len) {
			body.append(data, len);
			return true;
		});

		if (!res || res->status != 200)
			push({ WebEventType::GetFail, res ? res->status : -1, {} });
		else
			push({ WebEventType::GetComplete, res->status, std::move(body) });
	}).detach();
}

void WebManager::downloadFile(const std::string& url, const std::string& outPath) {
	if (downloading.exchange(true)) return;
	cancelled = false;
	progress.store(0.0f);
	speed.store(0.0f);

	std::thread([this, url, outPath] {
		namespace fs = std::filesystem;

		std::string path;
		auto cli = makeClient(url, path);
		if (!cli) {
			downloading = false;
			push({ WebEventType::DownloadFail, -1, {} });
			return;
		}

		bool extractAfter = !(outPath.size() > 4 && outPath.substr(outPath.size() - 4) == ".zip");
		fs::path downloadPath = extractAfter ? fs::path(outPath) / "tempLimeDownload.zip" : fs::path(outPath);

		std::error_code ec;
		fs::create_directories(downloadPath.parent_path(), ec);

		std::ofstream file(downloadPath, std::ios::binary);
		if (!file) {
			downloading = false;
			push({ WebEventType::DownloadFail, -1, {} });
			return;
		}

		httplib::Headers headers = {
			{ "User-Agent", "Mozilla/5.0" },
			{ "Accept", "*/*" }
		};

		int totalBytes = 0;
		int bytesWritten = 0;
		auto lastTick = std::chrono::steady_clock::now();
		int lastBytes = 0;

		auto res = cli->Get(
			path, headers,
			[&](const httplib::Response& r) {
				auto cl = r.get_header_value("Content-Length");
				if (!cl.empty()) totalBytes = (int)std::stoull(cl);
				return !cancelled.load();
			},
			[&](const char* data, size_t len) -> bool {
				if (cancelled) return false;

				file.write(data, len);
				bytesWritten += len;

				auto now = std::chrono::steady_clock::now();
				float window = std::chrono::duration<float>(now - lastTick).count();
				if (window >= 1.0f) {
					speed.store((bytesWritten - lastBytes) / window);
					lastBytes = bytesWritten;
					lastTick = now;
				}

				if (totalBytes > 0)
					progress.store(std::min(bytesWritten / (float)totalBytes, 0.99f));
				return true;
			}
		);

		file.close();

		if (cancelled) {
			fs::remove(downloadPath, ec);
			downloading = false;
			push({ WebEventType::DownloadFail, -1, {} });
			return;
		}

		int status = res ? res->status : -1;

		if (status != 200) {
			fs::remove(downloadPath, ec);
			downloading = false;
			push({ WebEventType::DownloadFail, status, {} });
			return;
		}

		if (extractAfter) {
			std::ifstream f(downloadPath, std::ios::binary);
			unsigned char sig[4]{};
			f.read(reinterpret_cast<char*>(sig), 4);
			bool isZip = f.gcount() == 4 && sig[0] == 'P' && sig[1] == 'K';
			f.close();

			if (!isZip || !extract(downloadPath.string(), outPath)) {
				fs::remove(downloadPath, ec);
				downloading = false;
				push({ WebEventType::DownloadFail, status, {} });
				return;
			}

			fs::remove(downloadPath, ec);
		}

		downloading = false;
		push({ WebEventType::DownloadComplete, status, extractAfter ? outPath : downloadPath.string() });
	}).detach();
}

bool WebManager::extract(const std::string& zipPath, const std::string& outDir) {
	mz_zip_archive zip{};
	if (!mz_zip_reader_init_file(&zip, zipPath.c_str(), 0)) return false;

	int fileCount = (int)mz_zip_reader_get_num_files(&zip);
	if (fileCount <= 0) { mz_zip_reader_end(&zip); return false; }

	std::error_code ec;
	std::filesystem::create_directories(outDir, ec);

	extracting = true;
	progress.store(0.0f);
	int extracted = 0;

	for (int i = 0; i < fileCount; ++i) {
		mz_zip_archive_file_stat stat{};
		if (!mz_zip_reader_file_stat(&zip, i, &stat)) continue;

		std::string name = stat.m_filename ? stat.m_filename : "";
		if (name.empty()) continue;

		std::filesystem::path dest = std::filesystem::path(outDir) / name;
		if (mz_zip_reader_is_file_a_directory(&zip, i)) {
			std::filesystem::create_directories(dest, ec);
			continue;
		}

		if (stat.m_bit_flag & 1) continue; // Encrypted...

		std::filesystem::create_directories(dest.parent_path(), ec);
		size_t size = 0;
		void* p = mz_zip_reader_extract_to_heap(&zip, i, &size, 0);
		if (!p) continue;

		std::ofstream out(dest, std::ios::binary);
		if (out) { 
			out.write(static_cast<const char*>(p), (std::streamsize)size);
			++extracted;
		}

		mz_free(p);

		progress.store((i + 1) / (float)fileCount);
	}

	mz_zip_reader_end(&zip);
	extracting = false;
	progress.store(1.0f);
	return extracted > 0;
}

