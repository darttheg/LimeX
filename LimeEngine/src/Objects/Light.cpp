#include "Objects/Light.h"

#include "Application.h"
#include "RenderHelper.h"
#include "DebugConsole.h"

#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"

#include "irrlicht.h"

static DebugConsole* d;
static RenderHelper* rh;

Light::Light() {
	light = rh->createLight();
	if (!light) return;
	light->getLightData().DiffuseColor = irr::video::SColor(255, 255, 0, 0);
	light->getLightData().AmbientColor = irr::video::SColor(255, 255, 0, 0);
	light->setLightType(irr::video::E_LIGHT_TYPE::ELT_POINT);
	light->getLightData().Radius = 500.0f;
	light->enableCastShadow(true);
;}

Light::Light(int type) : Light() {
	setType(type);
}

void Light::destroy() {
	if (light) light->remove();
	light = nullptr;
}

void Light::setDebug(bool v) {
}

irr::scene::ISceneNode* Light::getNode() const {
	return light;
}

int Light::getType() const {
	if (!light) return 0;
	return (int)light->getLightType();
}

void Light::setType(int type) {
	if (!light) return;
	light->setLightType((irr::video::E_LIGHT_TYPE)type);
}

Vec4 Light::getDiffuseColor() const {
	return Vec4();
}

void Light::setDiffuseColor(const Vec4& c) {

}

Vec4 Light::getAmbientColor() const {
	return Vec4();
}

void Light::setAmbientColor(const Vec4& c) {

}

Vec4 Light::getSpecularColor() const {
	return Vec4();
}

void Light::setSpecularColor(const Vec4& c) {

}

float Light::getIntensity() const {
	return 0.0f;
}

void Light::setIntensity(float i) {
}

Vec3 Light::getAttenuation() const {
	return Vec3();
}

void Light::setAttenuation(const Vec3& att) {
}

float Light::getRadius() const {
	return 0.0f;
}

void Light::setRadius(float r) {
}

float Light::getFalloff() const {
	return 0.0f;
}

void Light::setFalloff(float f) {
}

Vec2 Light::getCones() const {
	return Vec2();
}

void Light::setCones(const Vec2& c) {
}

bool Light::getShadows() const {
	return false;
}

void Light::setShadows(bool v) {

}

void Object::LightBind::bind(Application* a) {
	rh = a->GetRenderHelper();
	d = a->GetDebugConsole();

	// Object Light, A source of light.
	// Inherits Object3D

	sol::state_view view(a->GetLuaState());
	sol::usertype<Light> obj = view.new_usertype<Light>(
		"Light",
		sol::constructors<Light(), Light(int type)>(),

		sol::base_classes, sol::bases<Object3D>(),
		sol::meta_function::type, [](const Light&) { return "Light"; },

		// Field Lime.Enum.LightType type, Sets the type of this `Light`.
		"type", sol::property(&Light::getType, &Light::setType),

		// Field boolean shadows, Enables shadow-casting for this `Light`. `Mesh` objects must also have shadows enabled. (NOTE: Due to performance limitations, it is recommended to only have one main shadow-casting `Light`)
		"shadows", sol::property(&Light::getShadows, &Light::setShadows),

		// Field number intensity, Scales the intensity of luminosity, affecting only diffuse color.
		"intensity", sol::property(&Light::getIntensity, &Light::setIntensity),

		// Field number radius, The cut-off distance for light reach around its center. Not effective for directional light sources.
		"radius", sol::property(&Light::getRadius, &Light::setRadius),

		// Field number falloff, Dictates the blend from inner to outer cones for spotlights. For example, <1.0 is soft, 1.0 is linear, 2.0 is a bit harsher, 10.0+ is a very harsh cut-off. 
		"falloff", sol::property(&Light::getFalloff, &Light::setFalloff),

		// Field Vec4 diffuseColor, Sets the diffuse color for this `Light`, the main light color.
		"diffuseColor", sol::property(
			[](Light& c) { return Vec4{ [&] { return c.getDiffuseColor(); }, [&](auto v) { c.setDiffuseColor(v); } }; },
			[](Light& c, const Vec4& v) { c.setDiffuseColor(v); }
		),

		// Field Vec4 ambientColor, Sets the ambient color for this `Light`, the atmospheric color applied to all objects.
		"ambientColor", sol::property(
			[](Light& c) { return Vec4{ [&] { return c.getAmbientColor(); }, [&](auto v) { c.setAmbientColor(v); } }; },
			[](Light& c, const Vec4& v) { c.setAmbientColor(v); }
		),

		// Field Vec4 specularColor, Sets the specular color for this `Light`, the color that appears on shiny objects.
		"specularColor", sol::property(
			[](Light& c) { return Vec4{ [&] { return c.getSpecularColor(); }, [&](auto v) { c.setSpecularColor(v); } }; },
			[](Light& c, const Vec4& v) { c.setSpecularColor(v); }
		),

		// Field Vec3 attenuation, Sets the attenuation, or spread behavior, of this `Light`. Format is `(Constant, Linear, Quadratic)`, all ranging from 0.0 to 1.0. Not effective for directional light sources.
		"attenuation", sol::property(
			[](Light& c) { return Vec3{ [&] { return c.getAttenuation(); }, [&](auto v) { c.setAttenuation(v); } }; },
			[](Light& c, const Vec3& v) { c.setAttenuation(v); }
		),

		// Field Vec2 cones, Sets the inner and outer cones of this `Light`. This is only used for spotlights.
		"cones", sol::property(
			[](Light& c) { return Vec2{ [&] { return c.getCones(); }, [&](auto v) { c.setCones(v); } }; },
			[](Light& c, const Vec2& v) { c.setCones(v); }
		)
	);

	obj[sol::meta_function::to_string] = [](const Light& v) {
		return "Light";
		};

	// Constructor
	// Constructor Lime.Enum.LightType type

	// End Object
}