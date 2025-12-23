#pragma once

struct AppAlterables {
	std::string name = "app";
	std::string version = "1.0";
	std::string desc = "Lime application";
	std::string iconPath = "";
	std::string copyright = "";
};

void BuildPackage(const std::string& pDir, const std::string& oDir);