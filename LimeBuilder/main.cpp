// LimePlayer
// Compiles Lime projects into a package.

#include <iostream>
#include <chrono>
#include <iomanip>
#include <string>
#include <vector>
#include <filesystem>
#include <cctype>
#include "builder.h"
#include "BuildOutput.h"

AppAlterables parseArgs(int argc, char* argv[]) {
	AppAlterables out;

	for (int i = 1; i < argc; i++) {
		std::string a = argv[i];

		auto getVal = [&](int i) {
			if (i + 1 < argc) return std::string(argv[i + 1]);
			throw std::runtime_error("Missing value for argument " + a);
			};

		/*
		if (a == "--name") out.name = getVal(i++);
		else if (a == "--version") out.version = getVal(i++);
		else if (a == "--desc") out.desc = getVal(i++);
		else if (a == "--icon") out.iconPath = getVal(i++);
		else if (a == "--copyright") out.copyright = getVal(i++);
		*/
	}

	return out;
}

int main(int argc, char** argv) {
	auto start = std::chrono::steady_clock::now();

	std::vector<std::string> positional;
	bool packageOnly = false;
	std::string platform = "Windows";
	for (int i = 1; i < argc; i++) {
		std::string a = argv[i];
		if (a == "--package-only")
			packageOnly = true;
		else if (a == "--platform" && i + 1 < argc) {
			platform = argv[++i];
			if (!platform.empty()) platform[0] = (char)std::toupper((unsigned char)platform[0]);
		}
		else
			positional.push_back(a);
	}

	if (positional.empty()) {
		std::cout << "LimeBuilder <project dir> [output dir] [--package-only] [--platform Windows|Android]\n";
		return 0;
	}

	AppAlterables out = parseArgs(argc, argv);

	std::string pDir = positional[0];
	std::string oDir = positional.size() > 1 ? positional[1] : pDir;

	BuildOutput::Begin((std::filesystem::path(pDir) / "build.log").string(), platform, pDir);

	BuildResult result;
	try {
		result = BuildPackage(pDir, oDir, packageOnly);
	} catch (const std::exception& e) {
		BuildOutput::Fail(e.what());
		return 1;
	}

	auto end = std::chrono::steady_clock::now();
	double ms = std::chrono::duration<double, std::milli>(end - start).count();
	BuildOutput::Finish(result.output, result.modules, result.skipped, ms);

	return 0;
}