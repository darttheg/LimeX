#pragma once
#include <string>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#include <unordered_map>

class Window;
class DebugConsole;
class Renderer;

class Application {
public:
	Application();
	~Application();

	bool Init(const void* data, size_t size);
	bool Run();
	void EndApp();
	bool Stop();

	lua_State* GetLuaState() { return lua; }
	DebugConsole* GetDebugConsole() { return console; }
	bool IsRunning() { return running; }

private:
	bool CreateWindows();
	void DoLuaBinding();
	bool RunEntry();
	bool LoadPackage(const void* data, size_t size);

	// Lua
	lua_State* lua = nullptr;
	std::string entryModuleName;
	std::unordered_map<std::string, std::string> modules;

	// Debug console
	DebugConsole* console = nullptr;

	// Window & Renderer
	Window* window = nullptr;
	Renderer* renderer = nullptr;

	int width = 640;
	int height = 480;
	bool maximized = false;
	bool resizable = true;
	bool maintainAspect = false;
	bool vSync = false;
	std::string title = "LimeX Application";
	std::string iconPath; // Load in Window

	bool running = false;
};