#include "LuaBinder.h"
#include <sol/sol.hpp>
#include "Application.h"

// OBJECT INLCUDES
#include "ModuleLime.h"
// #include "ModuleScene.h"
//

void LuaBinder::BindAll(sol::state* lua, Application* app) {
	Module::Lime::init(app->GetDebugConsole(), app, app->GetRenderer());
	Module::Lime::bind(*lua);

	//Module::Scene::init(app->GetRenderer(), app->GetDebugConsole());
	//Module::Scene::bind(*lua);
}