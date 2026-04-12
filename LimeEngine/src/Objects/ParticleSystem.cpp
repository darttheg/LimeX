#include "Objects/ParticleSystem.h"

#include "RenderHelper.h"

#include "irrlicht.h"
#include <sol/sol.hpp>

#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"
#include "Objects/Material.h"

static RenderHelper* rh = nullptr;

ParticleSystem::ParticleSystem() {
	pfx = rh->createParticleSystem();
	applyPreset(0); // Change to point, apply default
}

ParticleSystem::ParticleSystem(int type) {
	pfx = rh->createParticleSystem();
	applyPreset(type);
}

void ParticleSystem::applyPreset(int type) {
	if (!pfx) return;
	type = std::clamp<int>(type, 0, 4);
}

bool ParticleSystem::getLocal() const {
	return pfx ? global : false;
}

void ParticleSystem::setLocal(bool v) {
	if (!pfx) return;
	pfx->setParticlesAreGlobal(v);
	global = v;
}

void ParticleSystem::clear() {
	if (!pfx) return;
	pfx->clearParticles();
}

void ParticleSystem::spark(int amount) {
	if (!pfx) return;
	pfx->spark(rh->getCurrentTime(), amount);
}

bool ParticleSystem::getActive() const {
	return pfx ? pfx->active : false;
}

void ParticleSystem::setActive(bool v) {
	if (!pfx) return;
	pfx->active = v;
}

void ParticleSystem::addAttractionAffector(const Vec3& pos, float spd, bool attract, const Vec3& affectAxis) {
	if (!pfx) return;
	irr::core::vector3df outPos(pos.getX(), pos.getY(), pos.getZ());
	irr::scene::IParticleAffector* out = pfx->createAttractionAffector(outPos, spd, attract, ceil(affectAxis.getX()) == 1, ceil(affectAxis.getY()) == 1, ceil(affectAxis.getZ()) == 1);
	pfx->addAffector(out);
}

void ParticleSystem::addFadeOutAffector(const Vec4& color, int timeMs) {
}

void ParticleSystem::addGravityAffector(const Vec3& grav, int timeToTakeOverMs) {
}

void ParticleSystem::addRotationAffector(const Vec3& rotSpd, const Vec3& pos) {
}

void ParticleSystem::addScalarAffector(float scalar) {
}

void ParticleSystem::clearAffectors() {
}

void ParticleSystem::loadMaterial(const Material& mat) {

}

Vec2 ParticleSystem::getRates() const {
	return Vec2();
}

void ParticleSystem::setRates(const Vec2&) {
}

float ParticleSystem::getSpread() const {
	return 0.0f;
}

void ParticleSystem::setSpread(float v) {
}

Vec2 ParticleSystem::getMinMaxLife() const {
	return Vec2();
}

void ParticleSystem::setMinMaxLife(const Vec2& v) {
}

Vec2 ParticleSystem::getMinMaxScale() const {
	return Vec2();
}

void ParticleSystem::setMinMaxScale(const Vec2& scale) {
}

Vec4 ParticleSystem::getMinColor() const {
	return Vec4();
}

void ParticleSystem::setMinColor(const Vec4& v) {
}

Vec4 ParticleSystem::getMaxColor() const {
	return Vec4();
}

void ParticleSystem::setMaxColor(const Vec4& v) {
}

Vec3 ParticleSystem::getBoxSize() const {
	return Vec3();
}

void ParticleSystem::setBoxSize(const Vec3& size) {
}

float ParticleSystem::getRadius() const {

}

void ParticleSystem::setRadius(float v) {

}

float ParticleSystem::getHeight() const {

}

void ParticleSystem::setHeight(float v) {

}

float ParticleSystem::getThickness() const {
	return 0.0f;
}

void ParticleSystem::setThickness(float v) {
}

void ParticleSystem::destroy() {
	if (pfx) pfx->remove();
	pfx = nullptr;
}

void ParticleSystem::setDebug(bool v) {
	if (v) {
		if (pfx) pfx->setDebugDataVisible(
			irr::scene::EDS_BBOX);
		if (dVisual) { dVisual->drop(); dVisual->remove(); }
		dVisual = rh->createDebugNode(DEBUG3D_TYPE::PARTICLES);
		dVisual->setParent(getNode());
	}
	else {
		if (pfx) pfx->setDebugDataVisible(irr::scene::EDS_OFF);
		if (dVisual) {
			dVisual->drop();
			dVisual->remove();
		}
	}
}

irr::scene::ISceneNode* ParticleSystem::getNode() const {
	return pfx;
}

void Object::ParticleSystemBind::bind(lua_State* ls, RenderHelper* renh) {
	rh = renh;

	// Object ParticleSystem, A particle emitter.
	// Inherits Object3D

	// Constructor

	sol::state_view view(ls);
	sol::usertype<ParticleSystem> obj = view.new_usertype<ParticleSystem>(
		"ParticleSystem",
		sol::constructors<ParticleSystem()>(),

		sol::base_classes, sol::bases<Object3D>(),
		sol::meta_function::type, [](const ParticleSystem&) { return "ParticleSystem"; }
	);

	obj[sol::meta_function::to_string] = [](const ParticleSystem& v) {
		return "ParticleSystem";
		};

	// End Object
}