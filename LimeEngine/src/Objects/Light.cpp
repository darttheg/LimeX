#include "Objects/Light.h"

#include "RenderHelper.h"

#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"

#include "irrlicht.h"
#include "Objects/DebugAxisPlaneNode.h"
#include <sol/sol.hpp>

static RenderHelper* rh;

Light::Light() {
	light = rh->createLight();
;}

Light::Light(int type) : Light() {
	setType(type);
}

void Light::destroy() {
	if (light) light->remove();
	light = nullptr;
}

void Light::setDebug(bool v) {
	if (v) {
		if (dVisual) dVisual->remove();
		dVisual = rh->createDebugNode(DEBUG3D_TYPE::LIGHT);
		dAxis->setPointerLength(light->getType() == 0 ? 0.0f : light->getLightData().Radius);
		dAxis->setPointerColor(light->getLightData().DiffuseColor);
	} else {
		if (dVisual) dVisual->remove();
	}
}

irr::scene::ISceneNode* Light::getNode() const {
	return light;
}

void Light::updateDiffuseColor() {
	if (!light) return;
	irr::video::SColorf ca = irr::video::SColorf(diffuse.x * intensity, diffuse.y * intensity, diffuse.z * intensity, diffuse.w * intensity);
	light->getLightData().DiffuseColor = ca;
}

int Light::getType() const {
	if (!light) return 0;
	return (int)light->getLightType();
}

void Light::setType(int type) {
	if (!light) return;
	light->setLightType((irr::video::E_LIGHT_TYPE)type);

	if (!dAxis) return;
	dAxis->setPointerLength(type == 0 ? 0.0f : 1.0f);
}

Vec4 Light::getDiffuseColor() const {
	if (!light) return Vec4();
	// irr::video::SColorf c = light->getLightData().DiffuseColor;
	return Vec4((int)diffuse.x * 255, (int)diffuse.y * 255, (int)diffuse.z * 255, (int)diffuse.w * 255);
}

void Light::setDiffuseColor(const Vec4& c) {
	if (!light) return;
	irr::video::SColorf ca = irr::video::SColorf(c.getX() / 255.0 * intensity, c.getY() / 255.0 * intensity, c.getZ() / 255.0 * intensity, c.getW() / 255.0 * intensity);
	diffuse = { ca.getRed(), ca.getGreen(), ca.getBlue(), ca.getAlpha() };
	light->getLightData().DiffuseColor = ca;
}

Vec4 Light::getAmbientColor() const {
	if (!light) return Vec4();
	irr::video::SColorf c = light->getLightData().AmbientColor;
	return Vec4((int)c.getRed() * 255, (int)c.getGreen() * 255, (int)c.getBlue() * 255, (int)c.getAlpha() * 255);
}

void Light::setAmbientColor(const Vec4& c) {
	if (!light) return;
	irr::video::SColorf ca = irr::video::SColorf(c.getW() / 255.0, c.getX() / 255.0, c.getY() / 255.0, c.getZ() / 255.0);
	light->getLightData().AmbientColor = ca;
	if (dAxis) dAxis->setPointerColor(light->getLightData().DiffuseColor);
}

Vec4 Light::getSpecularColor() const {
	if (!light) return Vec4();
	irr::video::SColorf c = light->getLightData().SpecularColor;
	return Vec4((int)c.getRed() * 255, (int)c.getGreen() * 255, (int)c.getBlue() * 255, (int)c.getAlpha() * 255);
}

void Light::setSpecularColor(const Vec4& c) {
	if (!light) return;
	irr::video::SColorf ca = irr::video::SColorf(c.getW() / 255.0, c.getX() / 255.0, c.getY() / 255.0, c.getZ() / 255.0);
	light->getLightData().SpecularColor = ca;
}

float Light::getIntensity() const {
	return light ? intensity : 0.0f;
}

void Light::setIntensity(float i) {
	if (!light) return;
	intensity = i;
	updateDiffuseColor();
}

Vec3 Light::getAttenuation() const {
	if (!light) return Vec3();
	irr::core::vector3df att = light->getLightData().Attenuation;
	return Vec3(att.X, att.Y, att.Z);
}

void Light::setAttenuation(const Vec3& att) {
	if (!light) return;
	irr::core::vector3df out = irr::core::vector3df(att.getX(), att.getY(), att.getZ());
	light->getLightData().Attenuation = out;
}

float Light::getRadius() const {
	return light ? light->getRadius() : 0.0f;
}

void Light::setRadius(float r) {
	if (!light) return;
	light->setRadius(r);
}

float Light::getFalloff() const {
	return light ? light->getLightData().Falloff : 0.0f;
}

void Light::setFalloff(float f) {
	if (!light) return;
	light->getLightData().Falloff = f;
}

Vec2 Light::getCones() const {
	return light ? Vec2(light->getLightData().InnerCone, light->getLightData().OuterCone) : Vec2();
}

void Light::setCones(const Vec2& c) {
	if (!light) return;
	light->getLightData().InnerCone = c.getX();
	light->getLightData().OuterCone = c.getY();
}

bool Light::getShadows() const {
	return light ? light->getLightData().CastShadows : false;
}

void Light::setShadows(bool v) {
	if (!light) return;
	light->getLightData().CastShadows = v;
}

void Object::LightBind::bind(lua_State* ls, RenderHelper* renh) {
	rh = renh;

	// Object Light, A source of light.
	// Inherits Object3D

	sol::state_view view(ls);
	sol::usertype<Light> obj = view.new_usertype<Light>(
		"Light",
		sol::constructors<Light(), Light(int type)>(),

		sol::base_classes, sol::bases<Object3D>(),
		sol::meta_function::type, [](const Light&) { return "Light"; },

		// Field Lime.Enum.LightType type, Sets the type of this `Light`.
		"type", sol::property(&Light::getType, &Light::setType),

		// Field boolean shadows, Enables shadow-casting for this `Light`. `Mesh` objects must also have shadows enabled. (NOTE: Due to performance limitations, it is recommended to only have one main shadow-casting `Light`)
		"shadows", sol::property(&Light::getShadows, &Light::setShadows),

		// Field number intensity, Scales the intensity of luminosity from this `Light`.
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