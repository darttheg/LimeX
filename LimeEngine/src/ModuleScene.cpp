#include "ModuleScene.h"

#include "Renderer.h"
#include "DebugConsole.h"
#include <sol/sol.hpp>

static Renderer* r;
static DebugConsole* d;

void Module::Scene::init(Renderer* rend, DebugConsole* deb) {
	if (!rend || !deb) return;

	r = rend;
	d = deb;
}

void Module::Scene::bind(sol::state& lua) {
	sol::table module = lua.create_named_table("Scene");
	lua["Lime"]["Scene"] = module;
}

// Functions