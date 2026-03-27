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
	// Params string path
	// Returns table
	module.set_function("getFilesInDirectory", &Module::File::Bind::getFilesInDir);

	// Returns true if `path` leads to a file.
	// Params string path
	// Returns boolean
	module.set_function("isFile", &Module::File::Bind::fileExists);

	// Returns contents of file at `path`.
	// Params string path
	// Returns string
	module.set_function("readFile", &Module::File::Bind::readFile);

	// Writes content to file at `path`. The file will be created if it does not exist.
	// Params string path
	// Returns boolean
	module.set_function("writeFile", &Module::File::Bind::writeFile);

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

bool Module::File::Bind::folderExists(std::string folderPath) {
	std::error_code ec;
	return std::filesystem::exists(folderPath, ec) && std::filesystem::is_directory(folderPath, ec);
}

sol::table Module::File::Bind::getFilesInDir(std::string directoryPath, std::string extension = "") {
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

bool Module::File::Bind::fileExists(std::string path) {
	std::error_code ec;
	return std::filesystem::exists(path, ec) && std::filesystem::is_regular_file(path, ec);
}

std::string Module::File::Bind::readFile(std::string path) {
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (!file.is_open()) return "";

	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}


bool Module::File::Bind::writeFile(std::string path, std::string data) {
	std::ofstream file(path, std::ios::out | std::ios::binary);
	if (!file.is_open()) return false;

	file.write(data.c_str(), data.size());
	return true;
}

std::string Module::File::Bind::getFileExtension(std::string path) {
	return std::filesystem::path(path).extension().string();
}

std::string Module::File::Bind::getFileName(std::string path) {
	return std::filesystem::path(path).filename().string();
}

bool Module::File::Bind::createDir(std::string path) {
	std::error_code ec;
	if (std::filesystem::exists(path, ec))
		return std::filesystem::is_directory(path, ec);
	return std::filesystem::create_directories(path, ec);
}