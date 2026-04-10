#pragma once
#include "Application.h"

#include <ctime>
#include <fstream>

#include "Window.h"
#include "DebugConsole.h"
#include "Receiver.h"
#include "Renderer.h"
#include "SoundManager.h"
#include "LuaBinder.h"
#include "Objects/Event.h"
#include "Objects/Vec2.h"
#include "FrameLimiter.h"
#include "PhysicsManager.h"

std::string readFile(const char* path) {
	std::ifstream file(path);
	if (!file.is_open()) return "";
	return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

uint16_t Application::LoadPackage(const void* data, size_t size) {
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
	else return 0;

	/*
	for (const auto& [name, code] : modules) {
		console->Log("Packaged module: " + name);
	}
	*/

	return count;
}

int Application::LuaPackageFinder(lua_State* l) {
	Application* app = static_cast<Application*>(lua_touserdata(l, lua_upvalueindex(1)));
	if (!app) return 1;

	const char* moduleName = luaL_checkstring(l, 1);
	auto it = app->modules.find(moduleName);
	if (it == app->modules.end()) {
		std::string err = "\n\tcould not find packaged module '" + std::string(moduleName) + "'";
		lua_pushlstring(l, err.c_str(), err.size());
		return 1;
	}

	const std::string& chunk = it->second;
	int status = luaL_loadbuffer(l, chunk.data(), chunk.size(), moduleName);
	if (status != LUA_OK) {
		std::string err = "\n\terror loading package module '" + std::string(moduleName) + "': " + lua_tostring(l, -1);
		lua_pop(l, 1);
		lua_pushlstring(l, err.c_str(), err.size());
		return 1;
	}

	return 1;
}

void Application::InstallPackageFinder() {
	lua_getglobal(GetLuaState(), "package");
	if (!lua_istable(GetLuaState(), -1)) {
		lua_pop(GetLuaState(), 1);
		return;
	}

	lua_getfield(GetLuaState(), -1, "searchers");
	if (!lua_istable(GetLuaState(), -1)) {
		lua_pop(GetLuaState(), 2);
		return;
	}

	lua_pushlightuserdata(GetLuaState(), this);
	lua_pushcclosure(GetLuaState(), Application::LuaPackageFinder, 1);

	lua_Integer len = static_cast<lua_Integer>(lua_rawlen(GetLuaState(), -2));
	
	for (lua_Integer i = len + 1; i > 2; --i) {
		lua_rawgeti(GetLuaState(), -2, i - 1);
		lua_rawseti(GetLuaState(), -3, i);
	}

	lua_rawseti(GetLuaState(), -2, 2);
	lua_pop(GetLuaState(), 2);
}

bool Application::RunEntry() {
	if (!lua) return false;

	if (entryModuleName.empty()) entryModuleName = "main";

	auto it = modules.find(entryModuleName);
	if (it == modules.end()) {
		console->PostError("Could not find entry module " + entryModuleName, true);
		displayMessage("Lime Init Error", "Could not find entry module " + entryModuleName, 1);
		return false;
	}

	const std::string& bc = it->second;

	sol::load_result chunk = lua->load_buffer(it->second.data(), it->second.size(), entryModuleName);
	if (!chunk.valid()) {
		sol::error err = chunk;
		console->PostError("Lua load error:\n" + std::string(err.what()), true);
		displayMessage("Lime Init Error", "Lua load error:\n" + std::string(err.what()), 1);
		return false;
	}

	sol::protected_function_result result = chunk();
	if (!result.valid()) {
		sol::error err = result;
		console->PostError("Lua entry run error:\n" + std::string(err.what()), true);
		displayMessage("Lime Init Error", "Lua entry run error:\n" + std::string(err.what()), 1);
		return false;
	}

	return true;
}

bool Application::Init(const void* data, size_t size, int argc, const char** argv) {
	// Create new Lua state

	// Allocate memory for now
	console = new DebugConsole(this);
	window = new Window(this);
	renderer = new Renderer(this);
	soundManager = new SoundManager(this);
	receiver = new Receiver(this, renderer->getGUIManager()); // Kind of odd, hopefully no issues in the future
	// Context: Without this ^, button events have to go from receiver, up to application, then through renderer to GUIManager...
	limiter = new FrameLimiter();

	lua = std::make_unique<sol::state>();

	if (!lua) {
		console->PostError("Failed to create Lua state", true);
		displayMessage("Lime Init Error", "Failed to create Lua state", 1);
		return false;
	}

	lua->open_libraries(
		sol::lib::base,
		sol::lib::string,
		sol::lib::os,
		sol::lib::utf8,
		sol::lib::io,
		sol::lib::math,
		sol::lib::table,
		sol::lib::package,
		sol::lib::debug
	);

	InstallPackageFinder();

	// Run randomseed using os time
	lua->script(R"(
		math.randomseed(os.time())
		math.random()
	)");

	// Bind objects
	DoLuaBinding();

	// Load Lua code
	uint16_t modulesAmount = LoadPackage(data, size);
	if (modulesAmount == 0) {
		console->PostError("Failed to load Lime package from memory", true);
		displayMessage("Lime Init Error", "Failed to load Lime package from memory", 1);
		return false;
	} else
		console->Log("Lime started with " + std::to_string(modulesAmount) + " modules loaded");

	// Parse commands
	parseCommandLine(argc, argv);

	// Run main file once
	if (!RunEntry())
		return false;

	// Run Init Event
	LimeInit.get()->engineRun([&](const std::string& msg) { console->PostError(msg); });

	if (!didInitCfg)
		console->Warn("Lime.setInitConfig was not called. Setting one-time parameters--such as driver type--can only be done via this function.");

	// Create device/true window using windowCfg
	if (!CreateWindows())
		return false;

	renderer->GetPhysicsManager()->SetLuaState(lua.get()->lua_state());

	if (!soundManager->Init()) {
		console->PostError("Failed to create sound manager", true);
		displayMessage("Lime Init Error", "Failed to create sound manager", 1);
		return false;
	}

	// Init console
	if (debugCfg.on) {
		console->Create();
		console->SetWriteOutput(debugCfg.write);
		if (debugCfg.suppress) console->Warn("Warnings are suppressed. Potential issues with this application will not be logged.", false);
		console->setSuppressWarnings(debugCfg.suppress);
	}

	return true;
}

#include <psapi.h>
int getMemUsed() {
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
	int physMemUsedMB = (int)physMemUsedByMe / (1024 * 1024) - 30;

	if (physMemUsedMB < 0) physMemUsedMB = 0;
	return physMemUsedMB;
}

#include <iostream>
bool Application::Run() {
	// Run application loop
	running = true;

	window->Focus();

	// Run Start Event
	LimeStart.get()->engineRun([&](const std::string& msg) { console->PostError(msg); });

	renderer->RenderBGPreUpdate();
	bool fail = false;
	double dt = 0.0f;
	while (running) {
		dt = limiter->beginFrame();

		console->Update(getMemUsed());

		if (window && window->ShouldClose()) { fail = true; }

		// Change to device->run -> update event -> render to fix event receiver
		if (!renderer->RunDevice()) { fail = true; }
		receiver->beginFrame();
		LimeUpdate.get()->engineRun([&](const std::string& msg) { console->PostError(msg); }, dt);
		window->PollEvents();
		soundManager->Update(dt);
		if (!renderer->Render(dt)) { fail = true; }

		// Clean-up
		updateFrameRate();
		renderer->EndWholeScene();
		receiver->endFrame();

		if (!windowCfg.vSync) limiter->endFrame();
		window->EndFrame(); // Swap buffers
		if (windowCfg.vSync) limiter->endFrame();

		if (fail) break;
	}

	LimeEnd.get()->engineRun([&](const std::string& msg) { console->PostError(msg); });

	Stop();
	return true;
}

void Application::EndApp() {
	// From LimePlayer function
	running = false;
}

bool Application::Stop() {
	std::string out = "Lime ended with ";
	out += std::to_string(console->GetWarningCount());
	out += " warnings, ";
	out += std::to_string(console->GetErrorCount());
	out += " errors";
	console->Log(out.c_str());

	console->Close(true);
	renderer->Shutdown();
	window->Close();
	return false;
}

RenderHelper* Application::GetRenderHelper() {
	return renderer ? renderer->GetRenderHelper() : nullptr;
}

GUIManager* Application::GetGUIManager() {
	return renderer ? renderer->getGUIManager() : nullptr;
}

PhysicsManager* Application::GetPhysicsManager() {
	return renderer ? renderer->GetPhysicsManager() : nullptr;
}

std::string Application::GetLuaLocation() {
	if (!lua) return "nan";
	lua_Debug ar{};
	int lvl = 1;

	if (lua_getstack(lua.get()->lua_state(), lvl, &ar) == 0) return "lime";
	if (lua_getinfo(lua.get()->lua_state(), "Sl", &ar) == 0) return "lime";

	std::string src = ar.source ? ar.source : "?";
	if (!src.empty() && src[0] == '@') src.erase(0, 1);

	int line = ar.currentline;
	if (line < 0) line = 0;

	std::ostringstream oss;
	oss << "[string \"" << src << "\"]:" << line;

	return oss.str();
}

void Application::setDebugConfig(bool on, bool write, bool suppress) {
	if (running) {
		console->Warn("Debug configuration must be called prior to window creation!");
		return;
	}

	debugCfg.on = on;
	debugCfg.write = write;
	debugCfg.suppress = suppress;
}

int Application::getMemoryUsage() {
	return getMemUsed();
}

void Application::parseCommandLine(int argc, const char** argv) {
	for (int i = 1; i < argc; ++i) {
		std::string key = argv[i];
		std::string value = "true";

		auto eqPos = key.find('=');
		if (eqPos != std::string::npos) {
			value = key.substr(eqPos + 1);
			key = key.substr(0, eqPos);
		}

		if (key.rfind("--", 0) == 0)
			key = key.substr(2);
		else if (key.rfind("-", 0) == 0)
			key = key.substr(1);

		if (eqPos == std::string::npos && i + 1 < argc && argv[i + 1][0] != '-')
			value = std::string(argv[++i]);

		if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') || (value.front() == '\'' && value.back() == '\''))) {
			value = value.substr(1, value.size() - 2);
		}

		cmd[key] = value;
	}
}

sol::object Application::getCommandLineValue(const std::string& key) {
	if (cmd.find(key) != cmd.end())
		return sol::make_object(GetLuaState(), cmd[key]);
	return sol::nil;
}

void Application::displayMessage(const std::string& title, const std::string message, int img) {
	std::wstring nTitle = std::wstring(title.begin(), title.end());
	const wchar_t* nTitleC = nTitle.c_str();

	std::wstring nMessage = std::wstring(message.begin(), message.end());
	const wchar_t* nMessageC = nMessage.c_str();

	UINT icon = MB_OK;
	img = irr::core::clamp<int>(img, 0, 3);

	switch (img) {
	case 0:
		icon = MB_OK;
		break;
	case 1:
		icon = MB_ICONWARNING;
		break;
	case 2:
		icon = MB_ICONQUESTION;
		break;
	case 3:
		icon = MB_ICONINFORMATION;
		break;
	}

	MessageBox(nullptr, nMessageC, nTitleC, icon);
}

bool Application::addArchive(const std::string& path) {
	return renderer->addArchive(path);
}

void Application::warnGarbageCollection(const std::string& path) {
	int max = 30;
	std::string out = path;
	if (path.length() > max) {
		int keep = max - 3;
		int start = path.length() - keep;
		int slashPos = path.find_first_of("/\\", start);
		if (slashPos != std::string::npos)
			out = "..." + path.substr(slashPos);
		else
			out + "..." + path.substr(start);
	}

	console->Warn("Object referencing asset " + out + " was garbage collected. Was it defined in a method as a local variable?", false);
}

void Application::setTargetFrameRate(int fps) {
	if (!limiter) return;
	windowCfg.frameRate = fps;
	limiter->setFPS(fps);
}

int Application::getFrameRate() {
	if (!limiter) return -1;
	return curFps > 0 ? curFps : GetRenderer()->getDriverFrameRate();
}

void Application::updateFrameRate() {
	if (!GetRenderer()) return;
	curFps = GetRenderer()->updateFrameRate();
}

void Application::setVSync(bool on) {
	if (!limiter) return;
	window->setSwapInterval(on ? 1 : 0);
	limiter->setFPS(window->getPrimaryHz());
	// limiter->setVSync(on);
	windowCfg.vSync = on;
}

bool Application::getVSync() {
	return windowCfg.vSync;
}

bool Application::CreateWindows() {
	if (!window->Create()) {
		console->PostError("Failed to create GLFW window", true);
		displayMessage("Lime Init Error", "Failed to create GLFW window", 1);
		return false;
	}

	if (!renderer->Init()) {
		console->PostError("Failed to create rendering window", true);
		displayMessage("Lime Init Error", "Failed to create rendering window", 1);
		return false;
	}

	if (!renderer->InitPhysics()) {
		console->PostError("Failed to create physics manager", true);
		displayMessage("Lime Init Error", "Failed to create physics manager", 1);
		return false;
	}

	HWND glfwHWND = window->GetHandle();

	if (!glfwHWND) {
		console->PostError("Could not get valid window handle(s)", true);
		displayMessage("Lime Init Error", "Could not get valid window handle(s)", 1);
		return false;
	}

	ShowWindow(glfwHWND, SW_SHOWNORMAL);
	SetForegroundWindow(glfwHWND);

	return true;
}

void Application::DoLuaBinding() {
	if (!lua) return;

	LuaBinder* binder = new LuaBinder();
	binder->BindAll(this);
	delete binder;
}
