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

	#define LIME_MODULE(name)
	#define LIME_FUNC(returnType, name, ...) \
		module.set_function(#name, &Module::Scene::name);

	#include "Scene.api.inl"
	#undef LIME_FUNC
	#undef LIME_MODULE
}

// Functions