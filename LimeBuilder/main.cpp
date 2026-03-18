// LimePlayer
// Compiles Lime projects into a package.

#include <iostream>
#include <chrono>
#include <iomanip>
#include "builder.h"

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

	if (argc < 2) {
		std::cout << "LimeBuilder <project dir> <output dir>\n";
		std::cout << "LimeBuilder <project + output dir>\n";
		return 0;
	}

	AppAlterables out = parseArgs(argc, argv);

	std::string pDir = argv[1];
	std::string oDir;
	if (argc == 2)
		oDir = pDir;
	else
		oDir = argv[2];

	try {
		BuildPackage(pDir, oDir);
	} catch (const std::exception& e) {
		std::cout << "\nFailed to build:\n  " << e.what() << "\n";
		return 1;
	}

	auto end = std::chrono::steady_clock::now();
	double ms = std::chrono::duration<double, std::milli>(end - start).count();

	if (ms < 1000.0)
		std::cout << "\Build complete in " << std::fixed << std::setprecision(1) << ms << "ms\n";
	else
		std::cout << "\Build complete in " << std::fixed << std::setprecision(2) << ms / 1000.0 << "s\n";

	return 0;
}