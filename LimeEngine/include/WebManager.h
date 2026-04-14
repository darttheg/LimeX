#pragma once
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "External/httplib.h"
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <memory>

class Application;
class Event;

enum class WebEventType {
	GetComplete,
	GetFail,
	DownloadComplete,
	DownloadFail
};

struct WebEvent {
	WebEventType type;
	int code;
	std::string data;
};

class WebManager {
public:
	bool Init(Application* owner);
	void Update();

	void get(const std::string& url);
	void downloadFile(const std::string& url, const std::string& outPath);
	void cancelDownload() { cancelled = true; }
	float getProgress() const { return progress.load(); }
	float getSpeed() const { return speed.load(); }
	bool isDownloading() const { return downloading.load(); }
	bool isExtracting() const { return extracting.load(); }
	void setTimeout(int ms) { timeOutMs = ms; }

	// Events
	std::shared_ptr<Event> LimeOnGetComplete = nullptr;
	std::shared_ptr<Event> LimeOnGetFail = nullptr;
	std::shared_ptr<Event> LimeOnDownloadComplete = nullptr;
	std::shared_ptr<Event> LimeOnDownloadFail = nullptr;
private:
	std::atomic<bool> downloading{ false };
	std::atomic<bool> cancelled{ false };
	std::atomic<bool> extracting{ false };
	std::atomic<float> progress{ 0.0f };
	std::atomic<float> speed{ 0.0f };
	int timeOutMs = 10000;

	std::mutex mu;
	std::vector<WebEvent> queue;

	void push(WebEvent e);
	void drain(std::vector<WebEvent>& out);

	std::unique_ptr<httplib::Client> makeClient(const std::string& url, std::string& outPath);
	bool extract(const std::string& zipPath, const std::string& outDir);
	
};