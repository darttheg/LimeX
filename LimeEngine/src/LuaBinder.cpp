#include "LuaBinder.h"
#include <sol/sol.hpp>
#include "Application.h"

// INTERFACE INCLUDES
#include "Interfaces/Object2D.h"
#include "Interfaces/Object3D.h"
//

// MODULE INLCUDES
#include "Modules/ModuleLime.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleWindow.h"
//

// OBJECT INCLUDES
#include "Objects/Event.h"
#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"
#include "Objects/Image.h"
#include "Objects/Material.h"
#include "Objects/Skydome.h"
#include "Objects/Camera.h"
//

void LuaBinder::BindAll(Application* app) {
	// Interfaces
	Interface::Object3DBind::bind(app);

	// Modules
	Module::Lime::bind(app);
	Module::Scene::bind(app);
	Module::Window::bind(app);

	// Objects
	Object::EventBind::bind(app);
	Object::Vec2Bind::bind(app);
	Object::Vec3Bind::bind(app);
	Object::Vec4Bind::bind(app);
	Object::ImageBind::bind(app);
	Object::MaterialBind::bind(app);
	Object::SkydomeBind::bind(app);
	Object::CameraBind::bind(app);
}