#include "LuaBinder.h"
#include <sol/sol.hpp>
#include "Application.h"

// MODULE INLCUDES
#include "Modules/ModuleLime.h"
//

// OBJECT INCLUDES
#include "Objects/Event.h"
#include "Objects/Vec2.h"
//

void LuaBinder::BindAll(Application* app) {
	// Objects
	Object::EventBind::bind(app);
	Object::Vec2Bind::bind(app);

	// Modules
	Module::Lime::bind(app);
}