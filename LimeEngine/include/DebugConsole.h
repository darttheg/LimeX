#pragma once

#include <string>

class Application;

enum struct MESSAGE_TYPE : int {
	NORMAL = 0,
	RED = 1,
	GREEN = 2,
	BLUE = 3
};

class DebugConsole {
public:
	DebugConsole();
	~DebugConsole();

	void Create();
	void SetAppOwner(Application* owner);
	void Close();
	void Log(const char* msg, MESSAGE_TYPE type = MESSAGE_TYPE::NORMAL);
	void Log(std::string msg, MESSAGE_TYPE type = MESSAGE_TYPE::NORMAL);
	void PostError(const char* msg, bool close = false);
	void PostError(std::string msg, bool close = false);
	void Update(int memMB);
	void WriteOutputLog();
	void SetEndOnError(bool v) { endOnError = v; }
	int GetMemUsed() { return memUsed; }
private:
	std::string out;
	bool created = false;
	bool doOutput = true;
	bool writeOutput = true;
	bool endOnError = false;
	int memUsed = 0;

	Application* app;
};