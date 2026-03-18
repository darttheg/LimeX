#include "Objects/Light.h"

#include "Application.h"
#include "RenderHelper.h"
#include "DebugConsole.h"

#include "irrlicht.h"

static DebugConsole* d;
static RenderHelper* rh;

Light::Light() {
}

void Light::destroy() {
}

void Light::setDebug(bool v) {
}

irr::scene::ISceneNode* Light::getNode() const {
	return light;
}

void Object::LightBind::bind(Application* a) {
	rh = a->GetRenderHelper();
	d = a->GetDebugConsole();

	// Object Light, A source of light.
	// Inherits Object3D

	sol::state_view view(a->GetLuaState());
	sol::usertype<Light> obj = view.new_usertype<Light>(
		"Light",
		sol::constructors<>(),

		sol::base_classes, sol::bases<Object3D>(),
		sol::meta_function::type, [](const Light&) { return "Light"; }
	);

	obj[sol::meta_function::to_string] = [](const Light& v) {
		return "Light";
		};

	// Constructor
	// Constructor Lime.Enum.LightType type

	// End Object
}