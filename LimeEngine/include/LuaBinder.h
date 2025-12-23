#pragma once
#include <string>
#include <sol/forward.hpp>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class Application;

class LuaBinder {
public:
	LuaBinder() = default;
	~LuaBinder() = default;

	void BindAll(sol::state* lua, Application* app);
private:
};