#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <Windows.h>

class Application;

enum struct MESSAGE_TYPE : int {
	NORMAL = 0,
	RED = 1,
	GREEN = 2,
	BLUE = 3,
	YELLOW = 4
};

class DebugConsole {
public:
	DebugConsole();
	~DebugConsole();

	void Create(); // Create console
	void Close(bool endApp = false); // Close console
	void Update(int memMB);

	int GetMemUsed() { return memUsed; }
	void SetEnable(bool v); // Enable/disable console
	void Clear(); // Clear lines

	void Log(const char* msg, MESSAGE_TYPE type = MESSAGE_TYPE::NORMAL);
	void Log(std::string msg, MESSAGE_TYPE type = MESSAGE_TYPE::NORMAL);
	void PostError(const char* msg, bool close = false);
	void PostError(std::string msg, bool close = false);
	void Warn(const char* msg);
	void Warn(std::string msg);

	void WriteOutputLog();
	void SetWriteOutput(bool v) { writeOutput = v; }
	void SetEndOnError(bool v) { endOnError = v; }
	void SetAppOwner(Application* owner);
private:
	Application* app;

	bool created = false;
	bool writeOutput = true;
	bool endOnError = false;
	int memUsed = 0;

	struct Line {
		Line(MESSAGE_TYPE m, std::string c) : type(m), content(std::move(c)) {}
		MESSAGE_TYPE type;
		std::string content;
	};

	std::vector<Line> consoleLines;

	void* consoleHandle = nullptr;
	unsigned short defaultAttr = 0;
	void AddLineToConsole(Line l);
};