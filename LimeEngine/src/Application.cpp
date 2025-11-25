#include "Application.h"

#include <ctime>
#include <fstream>

#include "Window.h"
#include "DebugConsole.h"
#include "Renderer.h"

Application::Application() {
}

Application::~Application() {
	if (lua) lua_close(lua);
}

std::string readFile(const char* path) {
	std::ifstream file(path);
	if (!file.is_open()) return "";
	return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

bool Application::LoadPackage(const void* data, size_t size) {
	modules.clear();

	if (!data || size < sizeof(uint32_t)) return false;

	const char* ptr = static_cast<const char*>(data);
	const char* end = ptr + size;

	auto readU32 = [&](uint32_t& out) -> bool {
		if (ptr + sizeof(uint32_t) > end) return false;
		memcpy(&out, ptr, sizeof(uint32_t));
		ptr += sizeof(uint32_t);
		return true;
	};

	uint32_t count = 0;
	if (!readU32(count)) return false;

	for (uint32_t i = 0; i < count; ++i) {
		uint32_t nameLen = 0;
		if (!readU32(nameLen)) return false;
		if (ptr + nameLen > end) return false;

		std::string name(ptr, nameLen);
		ptr += nameLen;

		uint32_t bcLen = 0;
		if (!readU32(bcLen)) return false;
		if (ptr + bcLen > end) return false;

		std::string bytecode(ptr, bcLen);
		ptr += bcLen;

		modules[name] = std::move(bytecode);
	}

	if (modules.count("main"))
		entryModuleName = "main";
	else if (!modules.empty())
		entryModuleName = modules.begin()->first;
	else return false;

	if (console) console->Log("Loaded package with " + std::to_string(count) + " modules.");

	return true;
}

bool Application::RunEntry() {
	if (!lua) return false;

	if (entryModuleName.empty()) entryModuleName = "main";

	auto it = modules.find(entryModuleName);
	if (it == modules.end()) {
		console->PostError("Could not find entry module " + entryModuleName, true);
		return false;
	}

	const std::string& bc = it->second;

	if (luaL_loadbuffer(lua, bc.data(), bc.size(), entryModuleName.c_str()) != LUA_OK) {
		std::string err = lua_tostring(lua, -1);
		lua_pop(lua, 1);
		console->PostError("Lua load error: " + err, true);
		return false;
	}

	if (lua_pcall(lua, 0, 0, 0) != LUA_OK) {
		std::string err = lua_tostring(lua, -1);
		lua_pop(lua, 1);
		console->PostError("Lua runtime error: " + err, true);
		return false;
	}

	return true;
}

bool Application::Init(const void* data, size_t size) {
	// Create debug console
	console = new DebugConsole;
	console->Create();

	// Create new Lua state
	lua = luaL_newstate();
	luaL_openlibs(lua);

	if (!lua) {
		console->PostError("Failed to create Lua state", true);
		return false;
	}

	// Run randomseed using os time
	std::string out;
	const char* rseed = "math.randomseed(os.time())\nmath.random()";
	if (luaL_dostring(lua, rseed) != LUA_OK) {
		std::string err = lua_tostring(lua, -1);
		lua_pop(lua, 1);
		console->PostError("Lua runtime error: " + err, true);
		return false;
	}

	// Bind objects
	DoLuaBinding();

	// Load Lua code
	if (!LoadPackage(data, size)) {
		console->PostError("Failed to load Lime package from memory", true);
		return false;
	}

	// Run main file once
	if (!RunEntry())
		return false;

	// Create device/true window
	if (!CreateWindows())
		return false;

	return true;
}

#include <psapi.h>
int getMemUsed() {
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
	int physMemUsedMB = (int)physMemUsedByMe / (1024 * 1024) - 10;

	if (physMemUsedMB < 0) physMemUsedMB = 0;
	return physMemUsedMB;
}

#include <iostream>
bool Application::Run() {
	// Run application loop
	running = true;
	if (console) console->Log("LimeX started");

	while (running) {
		console->Update(getMemUsed());
		window->PollEvents();

		// console->Log("running");

		if (window && window->ShouldClose()) break;
		if (!renderer->Render()) break;
	}

	Stop();
	return true;
}

void Application::EndApp() {
	// From LimePlayer function
	running = false;
}

bool Application::Stop() {
	console->Log("LimeX ended");

	console->Close();
	renderer->Close();
	window->Close();
	return false;
}

bool Application::CreateWindows() {
	window = new Window();
	if (!window->Create(width, height, title, maximized, resizable, maintainAspect)) {
		console->PostError("Failed to create GLFW window", true);
		return false;
	}

	renderer = new Renderer();
	renderer->SetDebugConsole(console);
	renderer->SetWindow(window);
	renderer->Create(4, 640, 480, false);

	// Parent windows
	HWND glfwHWND = window->GetHandle();
	HWND irrHWND = renderer->GetHandle();

	if (!glfwHWND || !irrHWND) {
		console->PostError("Could not get valid window handle(s)", true);
		return false;
	}

	if (irrHWND) {
		SetParent(irrHWND, glfwHWND);
		SetWindowLongPtr(irrHWND, GWL_STYLE, WS_CHILD | WS_VISIBLE);
		SetWindowPos(irrHWND, 0, 0, 0, width, height, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	}

	ShowWindow(glfwHWND, SW_RESTORE);
	SetForegroundWindow(glfwHWND);
	SetActiveWindow(glfwHWND);
	SendMessage(irrHWND, WM_ACTIVATE, WA_ACTIVE, 0);
	SendMessage(irrHWND, WM_SETFOCUS, 0, 0);

	return true;
}

void Application::DoLuaBinding() {
	console->Log("Doing Lua bindings...");
}
