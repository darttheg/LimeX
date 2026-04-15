#include "Modules/ModuleFile.h"

#include "Application.h"

#include <filesystem>
#include <fstream>
#include <sol/sol.hpp>

static Application* a;
static sol::state* l;

void Module::File::bind(Application* app) {
	a = app;
	l = &app->GetLuaState();
	sol::state& lua = app->GetLuaState();

	// Module Lime.File
	sol::table module = lua["Lime"]["File"].get_or_create<sol::table>();

	// Returns true if a directory exists at `path`.
	// Params string path
	// Returns boolean
	module.set_function("isDirectory", &Module::File::Bind::folderExists);

	// Returns a table of file paths from directory `path`.
	// Params string path, string? extension
	// Returns table
	module.set_function("getFilesInDirectory", &Module::File::Bind::getFilesInDir);

	// Returns true if `path` leads to a file.
	// Params string path
	// Returns boolean
	module.set_function("isFile", &Module::File::Bind::fileExists);

	// Returns contents of file at `path`. Optionally, a provided `seed` will attempt to decrypt the data.
	// Params string path
	// Params string path, string seed
	// Returns string
	module.set_function("readFile",
		sol::overload(
			sol::resolve<std::string(const std::string&)>(&Module::File::Bind::readFile),
			sol::resolve<std::string(const std::string&, const std::string&)>(&Module::File::Bind::readFileEncrypted)
		));

	// Writes content to file at `path`. The file will be created if it does not exist. Optionally, a provided `seed` will encrypt the data.
	// Params string path, string data
	// Params string path, string data, string seed
	// Returns boolean
	module.set_function("writeFile",
		sol::overload(
			sol::resolve<bool(const std::string&, const std::string&)>(&Module::File::Bind::writeFile),
			sol::resolve<bool(const std::string&, const std::string&, const std::string&)>(&Module::File::Bind::writeFileEncrypted)
		));

	// Returns the extension of the file at `path`.
	// Params string path
	// Returns string
	module.set_function("getExtension", &Module::File::Bind::getFileExtension);

	// Returns the name of the file at `path`.
	// Params string path
	// Returns string
	module.set_function("getFileName", &Module::File::Bind::getFileName);

	// Creates a directory.
	// Params string path
	// Returns boolean
	module.set_function("createDirectory", &Module::File::Bind::createDir);

	// End Module
}

// Functions

bool Module::File::Bind::folderExists(const std::string& folderPath) {
	std::error_code ec;
	return std::filesystem::exists(folderPath, ec) && std::filesystem::is_directory(folderPath, ec);
}

sol::table Module::File::Bind::getFilesInDir(const std::string& directoryPath, const std::string& extension) {
	sol::table result = l->create_table();
	std::error_code ec;

	if (!std::filesystem::exists(directoryPath, ec) || !std::filesystem::is_directory(directoryPath, ec))
		return result;

	int i = 1;
	for (const auto& entry : std::filesystem::directory_iterator(directoryPath, ec)) {
		if (ec) break;
		if (!entry.is_regular_file()) continue;

		if (extension.empty() || entry.path().extension().string() == extension) {
			result[i++] = entry.path().filename().string();
		}
	}

	return result;
}

bool Module::File::Bind::fileExists(const std::string& path) {
	std::error_code ec;
	return std::filesystem::exists(path, ec) && std::filesystem::is_regular_file(path, ec);
}

static void xorCipher(std::vector<uint8_t>& data, const std::string& seed) {
	if (seed.empty()) return;
	for (int i = 0; i < data.size(); i++)
		data[i] ^= seed[i % seed.size()];
}

std::string Module::File::Bind::readFile(const std::string& path) {
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (!file.is_open()) return "";

	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

std::string Module::File::Bind::readFileEncrypted(const std::string& path, const std::string& seed) {
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (!file.is_open()) return "";

	std::ostringstream ss;
	ss << file.rdbuf();
	std::string str = ss.str();
	std::vector<uint8_t> bytes(str.begin(), str.end());
	xorCipher(bytes, seed);
	return std::string(bytes.begin(), bytes.end());
}


bool Module::File::Bind::writeFile(const std::string& path, const std::string& data) {
	std::ofstream file(path, std::ios::out | std::ios::binary);
	if (!file.is_open()) return false;

	file.write(data.c_str(), data.size());
	return true;
}

bool Module::File::Bind::writeFileEncrypted(const std::string& path, const std::string& data, const std::string& seed) {
	std::ofstream file(path, std::ios::out | std::ios::binary);
	if (!file.is_open()) return false;
	
	std::vector<uint8_t> bytes(data.begin(), data.end());
	xorCipher(bytes, seed);
	file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
	return true;
}

std::string Module::File::Bind::getFileExtension(const std::string& path) {
	return std::filesystem::path(path).extension().string();
}

std::string Module::File::Bind::getFileName(const std::string& path) {
	return std::filesystem::path(path).filename().string();
}

bool Module::File::Bind::createDir(const std::string& path) {
	std::error_code ec;
	if (std::filesystem::exists(path, ec))
		return std::filesystem::is_directory(path, ec);
	return std::filesystem::create_directories(path, ec);
}