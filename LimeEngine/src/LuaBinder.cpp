#include "LuaBinder.h"
#include <sol/sol.hpp>
#include "Application.h"

// OBJECT INLCUDES
#include "ModuleLime.h"
#include "ModuleWorld.h"
//

void LuaBinder::BindAll(sol::state* lua, Application* app) {
	Module::Lime::init(app->GetDebugConsole(), app);
	Module::Lime::bind(*lua);

	// Module::World::init(app->GetRenderer(), app->GetDebugConsole());
	// Module::World::bind(*lua);
}