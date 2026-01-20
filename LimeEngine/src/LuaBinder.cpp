#include "LuaBinder.h"
#include <sol/sol.hpp>
#include "Application.h"

// MODULE INLCUDES
#include "Modules/ModuleLime.h"
//

// OBJECT INCLUDES
#include "Objects/Event.h"
//

void LuaBinder::BindAll(Application* app) {
	// Modules
	Module::Lime::bind(app);

	// Objects
	EventAndHook::bind(app);
}