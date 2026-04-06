#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

typedef void* (*FUNC_Lime_Create)();
typedef int (*FUNC_Lime_Run)(void* handle, int argc, const char** argv, const void* data, size_t size);
typedef void (*FUNC_Lime_End)(void* handle);

struct Footer {
	uint64_t offset;
	uint64_t size;
	char magic[8];
};

static bool LoadPkg(std::vector<char>& outData) {
	char path[MAX_PATH];
	DWORD len = GetModuleFileNameA(nullptr, path, MAX_PATH);
	if (len == 0 || len >= MAX_PATH) return false;

	std::ifstream f(path, std::ios::binary | std::ios::ate);
	if (!f.is_open()) return false;

	std::streamoff fileSize = f.tellg();
	if (fileSize < (std::streamoff)sizeof(Footer)) return false;

	f.seekg(fileSize - (std::streamoff)sizeof(Footer), std::ios::beg);

	Footer foot{};
	f.read(reinterpret_cast<char*>(&foot), sizeof(foot));
	if (!f.good()) return false;

	const char expected[8] = { 'L', 'I', 'M', 'E', 'P', 'K', 'G', '\0' };
	if (std::memcmp(foot.magic, expected, 8) != 0) return false;

	outData.resize((size_t)foot.size);
	f.seekg((std::streamoff)foot.offset, std::ios::beg);
	f.read(outData.data(), (std::streamsize)foot.size);
	if (!f.good()) return false;

	return true;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	SetDllDirectoryA("lib");
	HMODULE engine = LoadLibraryA("LimeEngine.dll");

	if (!engine) {
		MessageBoxA(NULL, "Missing LimeEngine.dll or associated app .dll files", "LimePlayer", MB_ICONERROR);
		return 1;
	}

	FUNC_Lime_Create Lime_Create = (FUNC_Lime_Create)GetProcAddress(engine, "Lime_Create");
	FUNC_Lime_Run Lime_Run = (FUNC_Lime_Run)GetProcAddress(engine, "Lime_Run");
	FUNC_Lime_End Lime_End = (FUNC_Lime_End)GetProcAddress(engine, "Lime_End");

	if (!Lime_Create || !Lime_Run || !Lime_End) {
		MessageBoxA(NULL, "LimeEngine.dll is missing required exports", "LimePlayer", MB_ICONERROR);
		return 1;
	}

	// Load package from memory
	std::vector<char> pkg;
	if (!LoadPkg(pkg)) {
		MessageBoxA(NULL, "Embedded Lime package not found or invalid", "LimePlayer", MB_ICONERROR);
		return 1;
	}

	void* handle = Lime_Create();
	if (!handle) {
		MessageBoxA(NULL, "Engine creation failed", "LimePlayer", MB_ICONERROR);
		return 1;
	}

	int argc = 0;
	LPWSTR* argvW = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (!argvW) {
		MessageBoxA(NULL, "Failed to parse command line", "LimePlayer", MB_ICONERROR);
		FreeLibrary(engine);
		return 1;
	}

	std::vector<std::string> argvStrings;
	std::vector<const char*> argv;

	argvStrings.reserve(argc);
	argv.reserve(argc);

	for (int i = 0; i < argc; ++i) {
		int needed = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, nullptr, 0, nullptr, nullptr);
		if (needed <= 0) {
			LocalFree(argvW);
			FreeLibrary(engine);
			MessageBoxA(NULL, "Failed to convert command line argument", "LimePlayer", MB_ICONERROR);
			return 1;
		}

		std::string s;
		s.resize((size_t)needed);

		int written = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, &s[0], needed, nullptr, nullptr);

		if (written <= 0) {
			LocalFree(argvW);
			FreeLibrary(engine);
			MessageBoxA(NULL, "Failed to convert command line argument", "LimePlayer", MB_ICONERROR);
			return 1;
		}

		if (!s.empty() && s.back() == '\0')
			s.pop_back();

		argvStrings.push_back(std::move(s));
		argv.push_back(argvStrings.back().c_str());
	}

	LocalFree(argvW);

	int result = Lime_Run(handle, argc, argv.data(), pkg.data(), pkg.size());

	Lime_End(handle);

	return result;
}