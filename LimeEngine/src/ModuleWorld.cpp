#include "ModuleWorld.h"

#include "Renderer.h"
#include "DebugConsole.h"
#include <sol/sol.hpp>

static Renderer* r;
static DebugConsole* d;

void Module::World::init(Renderer* rend, DebugConsole* deb) {
	if (!rend || !deb) return;

	r = rend;
	d = deb;
}

void Module::World::bind(sol::state& lua) {
	sol::table module = lua.create_named_table("World");
	lua["Lime"]["World"] = module;

	#define LIME_MODULE(name)
	#define LIME_FUNC(returnType, name, ...) \
		module.set_function(#name, &Module::World::name);

	#include "World.api.inl"
	#undef LIME_FUNC
	#undef LIME_MODULE
}

// Functions