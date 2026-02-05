#include "Modules/ModuleScene.h"

#include "DebugConsole.h"
#include "Application.h"
#include "Renderer.h"

#include "Objects/Event.h"

#include <sol/sol.hpp>

static DebugConsole* d;
static Application* a;
static Renderer* r;

void Module::Scene::bind(Application* app) {
	a = app;
	d = app->GetDebugConsole();
	r = app->GetRenderer();
	sol::state& lua = app->GetLuaState();

	// Module Lime.Scene
	sol::table module = lua["Lime"]["Scene"].get_or_create<sol::table>();

	// End Module
}

// Functions