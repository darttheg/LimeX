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
#include <vector>

class Window;
class DebugConsole;
class Renderer;
class Event;
class Receiver;
class FrameLimiter;
class RenderHelper;
class GUIManager;

#define LIME_VERSION "beta-0.1"

struct WindowConfig {
	int driverType = 0;
	std::vector<int> windowSize{ 640, 480 };
	std::vector<int> renderSize{ 640, 480 };
	int frameRate = 60;
	bool vSync = false;
	std::string title = "Lime App";
	bool maximizable = true;
	bool resizable = true;
	bool fullscreen = false;
	bool scaleRenderToWindow = true;
	bool keepAspectRatio = false;
};

class Application {
public:
	Application() = default;
	~Application() = default;

	std::shared_ptr<Event> LimeInit = nullptr;
	std::shared_ptr<Event> LimeStart = nullptr;
	std::shared_ptr<Event> LimeUpdate = nullptr;
	std::shared_ptr<Event> LimeEnd = nullptr;

	bool Init(const void* data, size_t size);
	bool Run();
	void EndApp();

	// Do not use to end from user! This is used by Lime.
	bool Stop();

	void DisplayMessage(std::string msg, std::string title, int icon);

	DebugConsole* GetDebugConsole() { return console; }
	Renderer* GetRenderer() { return renderer; }
	RenderHelper* GetRenderHelper();
	GUIManager* GetGUIManager();
	Window* GetWindow() { return window; }
	Receiver* GetReceiver() { return receiver; }
	bool IsRunning() { return running; }
	sol::state& GetLuaState() { return *lua; }
	
	std::string GetLuaLocation();

	WindowConfig GetConfig() { return windowCfg; }
	void SetConfig(WindowConfig cfg) { windowCfg = cfg; didInitCfg = true; }

	void setTargetFrameRate(int fps);
	int getFrameRate();
	void updateFrameRate();
	void setVSync(bool on);
	bool getVSync();
	void setDebugConfig(bool on, bool write = false);
	int getMemoryUsage();

private:
	bool CreateWindows();
	void DoLuaBinding();
	bool RunEntry();
	uint16_t LoadPackage(const void* data, size_t size);

	// Lua
	std::unique_ptr<sol::state> lua;
	std::string entryModuleName;
	std::unordered_map<std::string, std::string> modules;

	// Debug console
	struct DebugConfig { bool on, write; };
	DebugConfig debugCfg{};
	DebugConsole* console = nullptr;

	// Window & Renderer
	Window* window = nullptr;
	Renderer* renderer = nullptr;
	FrameLimiter* limiter = nullptr;
	int curFps = 0;

	// Window parameters
	WindowConfig windowCfg;
	bool didInitCfg = false;
	bool running = false;

	// Input
	Receiver* receiver = nullptr;
};