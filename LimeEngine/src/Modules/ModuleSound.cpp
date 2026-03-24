#include "Modules/ModuleSound.h"

#include "Application.h"
#include "SoundManager.h"
#include "Objects/Vec2.h"
#include "Objects/Vec3.h"

#include <sol/sol.hpp>

static Application* a;

void Module::Sound::bind(Application* app) {
	a = app;
	sol::state& lua = app->GetLuaState();

	// Module Lime.Sound

	// End Module
}

// Functions