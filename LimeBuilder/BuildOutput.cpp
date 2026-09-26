#include "BuildOutput.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <io.h>
#include <Windows.h>

namespace {
	std::ofstream logFile;
	int total = 1, done = 0, warnings = 0;
	bool fancy = false;
	UINT oldCp = 0;
	std::string lastLabel;
	std::string logPathFull;

	const char* c(const char* code) { return fancy ? code : ""; }
	const char* LIME() { return c("\x1b[38;2;163;230;53m"); }
	const char* DIM() { return c("\x1b[90m"); }
	const char* RED() { return c("\x1b[91m"); }
	const char* YELLOW() { return c("\x1b[93m"); }
	const char* BOLD() { return c("\x1b[1m"); }
	const char* RESET() { return c("\x1b[0m"); }

	bool enableConsole() {
		if (!_isatty(_fileno(stdout))) return false;
		HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD mode = 0;
		if (!GetConsoleMode(out, &mode)) return false;
		if (!SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) return false;
		oldCp = GetConsoleOutputCP();
		SetConsoleOutputCP(CP_UTF8);
		return true;
	}

	void restoreConsole() {
		if (oldCp) SetConsoleOutputCP(oldCp);
	}

	void drawBar(const std::string& label) {
		if (!fancy) return;
		const int width = 28;
		const int filled = total > 0 ? done * width / total : width;
		const int pct = total > 0 ? done * 100 / total : 100;
		std::string on, off;
		for (int i = 0; i < width; ++i) (i < filled ? on : off) += i < filled ? "\xE2\x94\x81" : "\xE2\x94\x80";
		std::cout << "\r\x1b[2K" << LIME() << on << RESET() << DIM() << off << RESET() << "  " << pct << "%";
		if (!label.empty()) std::cout << "  " << DIM() << label << RESET();
		std::cout << std::flush;
	}

	std::string fmtMs(double ms) {
		std::ostringstream s;
		if (ms < 1000.0) s << std::fixed << std::setprecision(1) << ms << "ms";
		else s << std::fixed << std::setprecision(2) << ms / 1000.0 << "s";
		return s.str();
	}

	std::string logLink() {
		return "\"" + logPathFull + "\"";
	}
}

void BuildOutput::Begin(const std::string& logPath, const std::string& platform, const std::string& projectDir) {
	logFile.open(logPath, std::ios::trunc);
	std::error_code ec;
	logPathFull = std::filesystem::absolute(logPath, ec).string();
	if (ec) logPathFull = logPath;
	std::time_t t = std::time(nullptr);
	std::tm tm{};
	localtime_s(&tm, &t);
	logFile << "Lime build " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "\n";
	logFile << "Compiling " << platform << " project from root " << projectDir << "...\n";

	fancy = enableConsole();
	std::string name = std::filesystem::path(projectDir).filename().string();
	if (name.empty()) name = std::filesystem::path(projectDir).parent_path().filename().string();
	std::cout << "\n" << BOLD() << LIME() << "Lime" << RESET() << ": " << platform << " (" << name << ")\n";
}

void BuildOutput::Log(const std::string& line) {
	logFile << line << "\n";
}

void BuildOutput::Warn(const std::string& line) {
	logFile << "WARNING: " << line << "\n";
	++warnings;
}

void BuildOutput::SetTotal(int steps) {
	total = steps > 0 ? steps : 1;
	done = 0;
}

void BuildOutput::Step(const std::string& label) {
	lastLabel = label;
	drawBar(label);
	if (done < total) ++done;
}

void BuildOutput::Finish(const std::string& outputPath, int modules, int skipped, double ms) {
	logFile << "Build complete in " << fmtMs(ms) << "\n";
	logFile.close();

	done = total;
	drawBar("");
	if (fancy) std::cout << "\n";

	std::cout << "\nCompiled " << modules << (modules == 1 ? " module" : " modules");
	if (skipped) std::cout << ", " << skipped << " skipped";
	std::cout << ", " << fmtMs(ms) << " elapsed\n";
	if (warnings) std::cout << YELLOW() << warnings << (warnings == 1 ? " warning" : " warnings") << RESET() << "\n";
	std::cout << "Details in " << logLink() << "\n";
	restoreConsole();
}

void BuildOutput::Fail(const std::string& error) {
	logFile << "Build failed: " << error << "\n";
	logFile.close();

	drawBar(lastLabel);
	if (fancy) std::cout << "\n";
	std::cout << "\n" << RED() << "Build failed: " << RESET() << error << "\n";
	std::cout << "Details in " << logLink() << "\n";
	restoreConsole();
}
