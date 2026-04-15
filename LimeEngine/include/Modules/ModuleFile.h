#pragma once

#include <string>const std::string& 
#include <sol/forward.hpp>
class Application;

namespace Module {
	namespace File {
		void bind(Application* app);

		namespace Bind {
			bool folderExists(const std::string& folderPath);
			sol::table getFilesInDir(const std::string& directoryPath, const std::string& extension = "");
			bool fileExists(const std::string& path);
			std::string readFile(const std::string& path);
			std::string readFileEncrypted(const std::string& path, const std::string& seed);
			bool writeFile(const std::string& path, const std::string& data);
			bool writeFileEncrypted(const std::string& path, const std::string& data, const std::string& seed);
			std::string getFileExtension(const std::string& path);
			std::string getFileName(const std::string& path);
			bool createDir(const std::string& path);
		}
	}
}