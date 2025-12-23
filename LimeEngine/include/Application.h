#pragma once

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#include <string>
#include <sol/sol.hpp>
#include <unordered_map>
#include <memory>

class Window;
class DebugConsole;
class Renderer;

#define LIME_VERSION "1.0"

class Application {
public:
	Application() = default;
	~Application() = default;

	bool Init(const void* data, size_t size);
	bool Run();
	void EndApp();

	// Do not use to end from user!
	bool Stop();

	void DisplayMessage(std::string msg, std::string title, int icon);

	DebugConsole* GetDebugConsole() { return console; }
	Renderer* GetRenderer() { return renderer; }
	bool IsRunning() { return running; }

private:
	bool CreateWindows();
	void DoLuaBinding();
	bool RunEntry();
	bool LoadPackage(const void* data, size_t size);

	// Lua
	std::unique_ptr<sol::state> lua;
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
	std::string title = "Lime Application";
	std::string iconPath; // Load in Window

	bool running = false;
};