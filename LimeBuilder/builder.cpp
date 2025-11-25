#include <filesystem>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <regex>
#include <Windows.h>

#include "builder.h"

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

namespace fs = std::filesystem;

struct LuaModule {
	std::string name;
	std::string code;
	std::string bytecode;
};

static std::vector<LuaModule> modules;
static std::unordered_map<std::string, LuaModule*> modulesByName;

static std::string readModule(const fs::path& p) {
	std::ifstream f(p, std::ios::binary);
	if (!f.is_open())
		return {};

	return std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}

static std::string toModuleName(const fs::path& src, const fs::path& file) {
	fs::path rel = fs::relative(file, src);
	std::string out = rel.string();
	std::replace(out.begin(), out.end(), '\\', '/');

	// Remove end
	if (out.size() >= 6 && out.substr(out.size() - 6) == ".lua")
		out.erase(out.size() - 6);

	std::replace(out.begin(), out.end(), '/', '.');

	return out;
}

static int writer(lua_State* L, const void* p, size_t size, void* ud) {
	std::string* out = static_cast<std::string*>(ud);
	out->append(static_cast<const char*>(p), size);
	return 0;
}

std::string compileLuaToBC(lua_State* L, const std::string& code, const std::string& name) {
	if (luaL_loadbuffer(L, code.c_str(), code.size(), name.c_str()) != LUA_OK)
		throw std::runtime_error(lua_tostring(L, -1));

	std::string bytecode;
	if (lua_dump(L, writer, &bytecode, 0) != 0)
		throw std::runtime_error("lua_dump failed");

	lua_pop(L, 1);

	return bytecode;
}

static void resolveRequires() {
	std::regex pattern(R"(require\s*\(\s*["']([^"']+)["']\s*\))");

	for (auto& m : modules) {
		auto begin = std::sregex_iterator(m.code.begin(), m.code.end(), pattern);
		auto end = std::sregex_iterator();

		for (auto it = begin; it != end; ++it) {
			std::string required = (*it)[1].str();
			if (!modulesByName.count(required))
				throw std::runtime_error("Module '" + m.name + "' requires module '" + required + "' but it is missing!");
		}
	}
}

struct Footer {
	uint64_t offset;
	uint64_t size;
	char magic[8];
};

bool LoadEmbeddedPlayer(std::vector<char>& out) {
	HRSRC res = FindResourceA(NULL, "LIMEPLAYER", MAKEINTRESOURCEA(10));
	if (!res) return false;

	HGLOBAL hRes = LoadResource(NULL, res);
	if (!hRes) return false;

	void* ptr = LockResource(hRes);
	DWORD size = SizeofResource(NULL, res);

	if (!ptr || size == 0) return false;

	out.resize(size);
	memcpy(out.data(), ptr, size);

	return true;
}

void EmbedPackage(const std::string& exePath, const std::string& pkgPath) {
	std::ifstream pkg(pkgPath, std::ios::binary | std::ios::ate);
	if (!pkg.is_open())
		throw std::runtime_error("Failed to open package: " + pkgPath);

	std::streamoff pkgSize = pkg.tellg();
	pkg.seekg(0, std::ios::beg);

	std::vector<char> pkgData((size_t)pkgSize);
	if (!pkg.read(pkgData.data(), pkgSize))
		throw std::runtime_error("Failed to read package: " + pkgPath);

	std::fstream exe(exePath, std::ios::binary | std::ios::in | std::ios::out | std::ios::ate);
	if (!exe.is_open())
		throw std::runtime_error("Failed to open LimePlayer exe: " + exePath);

	std::streamoff exeSize = exe.tellg();
	exe.write(pkgData.data(), (std::streamsize)pkgData.size());

	Footer foot{};
	foot.offset = (std::uint64_t)exeSize;
	foot.size = (std::uint64_t)pkgData.size();
	const char magic[8] = { 'L', 'I', 'M', 'E', 'P', 'K', 'G', '\0' };

	std::memcpy(foot.magic, magic, 8);

	exe.write(reinterpret_cast<const char*>(&foot), sizeof(foot));
	exe.close();
}

void BuildPackage(const std::string& pDir, const std::string& oDir) {
	modules.clear();
	modulesByName.clear();
	fs::path src = fs::path(pDir);
	std::cout << "Searching " << src.string() << "...\n";

	bool hasMain = false;
	std::string mainFull;

	for (auto& entry : fs::recursive_directory_iterator(src)) {
		if (!entry.is_regular_file())
			continue;

		if (entry.path().extension() != ".lua")
			continue;

		LuaModule m;
		m.name = toModuleName(src, entry.path());
		m.code = readModule(entry.path());
		if (m.code.empty()) {
			std::cerr << "WARNING: Could not read " << entry.path().string() << "\n";
			continue;
		}

		if (modulesByName.count(m.name))
			throw std::runtime_error("There are multiple '" + m.name + "' modules!");

		if (entry.path().filename() == "main.lua") {
			hasMain = true;
			mainFull = m.name;
		}

		modules.push_back(std::move(m));
		modulesByName[modules.back().name] = &modules.back();
	}

	if (modules.empty())
		throw std::runtime_error("No modules could be found!");

	if (!hasMain)
		throw std::runtime_error("No 'main.lua' found!");

	resolveRequires();

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	for (auto& m : modules) {
		try {
			std::cout << "Compiling " << m.name << "\n";
			m.bytecode = compileLuaToBC(L, m.code, m.name);
		}
		catch (const std::exception& e) {
			throw std::runtime_error("Failed to compile module '" + m.name + "': " + e.what());
		}
	}

	std::cout << "Compiled " << modules.size() << " modules\n";

	fs::create_directories(oDir);
	fs::path outPkg = fs::path(oDir) / "app.limepkg";
	std::ofstream f(outPkg, std::ios::binary);

	if (!f.is_open())
		throw std::runtime_error("Could not write output Lime package!");

	uint32_t count = (uint32_t)modules.size();
	f.write((char*)&count, sizeof(count));

	for (auto& m : modules) {
		uint32_t nameLen = (uint32_t)m.name.size();
		uint32_t bcLen = (uint32_t)m.bytecode.size();

		f.write((char*)&nameLen, sizeof(nameLen));
		f.write(m.name.data(), nameLen);

		f.write((char*)&bcLen, sizeof(bcLen));
		f.write(m.bytecode.data(), bcLen);
	}
	f.close();

	std::cout << "Created Lime package at: " << outPkg.string() << "\n";

	std::vector<char> templateExe;
	if (!LoadEmbeddedPlayer(templateExe))
		throw std::runtime_error("Failed to load LimePlayer executable");

	fs::path finalPath = fs::path(oDir) / "app.exe";
	std::string finalExe = finalPath.string();

	std::ofstream exe(finalExe, std::ios::binary | std::ios::trunc);
	exe.write(templateExe.data(), templateExe.size());
	exe.close();

	EmbedPackage(finalExe, outPkg.string());

	std::cout << "Created application exe at:\n  " << finalExe << "\n";
}