#pragma once
#include <string>

namespace BuildOutput {
	void Begin(const std::string& logPath, const std::string& platform, const std::string& projectDir);
	void Log(const std::string& line);
	void Warn(const std::string& line);
	void SetTotal(int steps);
	void Step(const std::string& label);
	void Finish(const std::string& outputPath, int modules, int skipped, double ms);
	void Fail(const std::string& error);
}
