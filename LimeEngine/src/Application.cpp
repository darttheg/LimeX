#include "Application.h"

#include <ctime>
#include <fstream>

#include "Window.h"
#include "DebugConsole.h"
#include "Renderer.h"
#include "LuaBinder.h"
#include "Objects/Event.h"

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

	return count;
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

	sol::load_result chunk = lua->load_buffer(it->second.data(), it->second.size(), entryModuleName);
	if (!chunk.valid()) {
		sol::error err = chunk;
		console->PostError("Lua load error:\n" + std::string(err.what()), true);
		return false;
	}

	sol::protected_function_result result = chunk();
	if (!result.valid()) {
		sol::error err = result;
		console->PostError("Lua entry run error:\n" + std::string(err.what()), true);
		return false;
	}

	return true;
}

bool Application::Init(const void* data, size_t size) {
	// Create debug console
	console = new DebugConsole;
	// We init it after the window is created

	// console->Log("Lime started");

	renderer = new Renderer; // Allocate memory for now

	console->SetAppOwner(this);

	// Create new Lua state
	lua = std::make_unique<sol::state>();
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

	if (!lua) {
		console->PostError("Failed to create Lua state", true);
		return false;
	}

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
		return false;
	} else
		console->Log("Lime started with " + std::to_string(modulesAmount) + " modules loaded");

	// Run main file once
	if (!RunEntry())
		return false;

	// Run Init Event
	LimeInit.get()->engineRun(GetLuaState(), [&](const std::string& msg) { console->PostError(msg); });

	if (!didInitCfg)
		console->Warn("Lime.SetInitConfig was not called. Setting one-time parameters--such as driver type--can only be done via this function.");

	// Create device/true window
	if (!CreateWindows())
		return false;

	// Init console
	console->Create();

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

	bool fail = false;
	while (running) {
		window->PollEvents();

		// console->Log("running");
		console->Update(getMemUsed());

		LimeUpdate.get()->engineRun(GetLuaState(), [&](const std::string& msg) { console->PostError(msg); });

		if (window && window->ShouldClose()) fail = true;
		if (!renderer->Render()) fail = true;

		if (fail) break;
	}

	LimeEnd.get()->engineRun(GetLuaState(), [&](const std::string& msg) { console->PostError(msg); });

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

void Application::DisplayMessage(std::string msg, std::string title, int icon) {
	std::wstring nTitle = std::wstring(title.begin(), title.end());
	const wchar_t* nTitleC = nTitle.c_str();

	std::wstring nMessage = std::wstring(msg.begin(), msg.end());
	const wchar_t* nMessageC = nMessage.c_str();

	UINT oIcon = MB_OK;

	switch (icon) {
	case 1:
		oIcon = MB_ICONWARNING;
		break;
	case 2:
		oIcon = MB_ICONQUESTION;
		break;
	case 3:
		oIcon = MB_ICONINFORMATION;
		break;
	default:
		oIcon = MB_OK;
	}

	MessageBox(nullptr, nMessageC, nTitleC, oIcon);
}

bool Application::CreateWindows() {
	window = new Window(this);
	if (!window->Create()) {
		console->PostError("Failed to create GLFW window", true);
		return false;
	}

	if (!renderer->Init(console, window)) {
		console->PostError("Failed to create rendering window", true);
		return false;
	}

	HWND glfwHWND = window->GetHandle();

	if (!glfwHWND) {
		console->PostError("Could not get valid window handle(s)", true);
		return false;
	}

	return true;
}

void Application::DoLuaBinding() {
	if (!lua) return;

	LuaBinder* binder = new LuaBinder();
	binder->BindAll(this);
	delete binder;
}
