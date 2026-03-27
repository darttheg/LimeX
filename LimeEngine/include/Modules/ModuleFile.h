#pragma once

#include <string>
#include <sol/forward.hpp>
class Application;

namespace Module {
	namespace File {
		void bind(Application* app);

		namespace Bind {
			bool folderExists(std::string folderPath);
			sol::table getFilesInDir(std::string directoryPath, std::string extension);
			bool fileExists(std::string path);
			std::string readFile(std::string path);
			bool writeFile(std::string path, std::string data);
			std::string getFileExtension(std::string path);
			std::string getFileName(std::string path);
			bool createDir(std::string path);
		}
	}
}