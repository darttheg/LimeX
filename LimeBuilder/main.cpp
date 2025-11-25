// LimePlayer
// Compiles LimeX projects into a package.

#include <iostream>
#include "builder.h"

AppAlterables parseArgs(int argc, char* argv[]) {
	AppAlterables out;

	for (int i = 1; i < argc; i++) {
		std::string a = argv[i];

		auto getVal = [&](int i) {
			if (i + 1 < argc) return std::string(argv[i + 1]);
			throw std::runtime_error("Missing value for argument " + a);
			};

		if (a == "--name") out.name = getVal(i++);
		else if (a == "--version") out.version = getVal(i++);
		else if (a == "--desc") out.desc = getVal(i++);
		else if (a == "--icon") out.iconPath = getVal(i++);
		else if (a == "--copyright") out.copyright = getVal(i++);
	}

	return out;
}

int main(int argc, char** argv) {
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
		return 0;
	}

	std::cout << "Your LimeX project has been built to\n  " << oDir << "\n";

	return 0;
}