#pragma once
#include <string>

struct AppAlterables {
	std::string name = "app";
	std::string version = "1.0";
	std::string desc = "Lime application";
	std::string iconPath = "";
	std::string copyright = "";
};

struct BuildResult {
	std::string output;
	int modules = 0;
	int skipped = 0;
};

BuildResult BuildPackage(const std::string& pDir, const std::string& oDir, bool packageOnly = false);