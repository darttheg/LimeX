#include "Application.h"

#include <ctime>
#include <fstream>

#include "Window.h"
#include "DebugConsole.h"
#include "Renderer.h"
#include "LuaBinder.h"

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
	console->Create();

	console->Log("Lime started");

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
	if (console) console->Log("Lime is rendering");

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
	console->Log("Lime ended");

	console->Close();
	renderer->Close();
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
	window = new Window();
	if (!window->Create(width, height, title, maximized, resizable, maintainAspect)) {
		console->PostError("Failed to create GLFW window", true);
		return false;
	}

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
	if (!lua) return;

	LuaBinder* binder = new LuaBinder();
	binder->BindAll(lua.get(), this);
	delete binder;
}
