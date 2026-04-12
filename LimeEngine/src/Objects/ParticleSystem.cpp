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
	setType(0);
}

ParticleSystem::ParticleSystem(int type) {
	pfx = rh->createParticleSystem();
	setType(type);
}

int ParticleSystem::getType() const {
	return pfx ? curType : 0;
}

void ParticleSystem::setType(int type) {
	if (!pfx) return;
	curType = type;

	irr::scene::IParticleEmitter* em = nullptr;
	switch (type) {
	case 0:
		em = pfx->createPointEmitter();
		break;
	case 1:
		em = pfx->createBoxEmitter();
		break;
	case 2:
		em = pfx->createSphereEmitter(irr::core::vector3df(), 5);
		break;
	case 3:
		em = pfx->createRingEmitter(irr::core::vector3df(), 5, 1);
		break;
	case 4:
		em = pfx->createCylinderEmitter(irr::core::vector3df(), 5, irr::core::vector3df(0,1,0), 5);
		break;
	}

	if (em) {
		pfx->setEmitter(em);
		em->drop();
		loadUserParams();
	}
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
	if (!pfx) return;
	irr::scene::IParticleAffector* out = pfx->createFadeOutParticleAffector(irr::video::SColor(color.getW(), color.getX(), color.getY(), color.getZ()), timeMs);
	pfx->addAffector(out);
}

void ParticleSystem::addGravityAffector(const Vec3& grav, int timeToTakeOverMs) {
	if (!pfx) return;
	irr::scene::IParticleAffector* out = pfx->createGravityAffector(irr::core::vector3df(grav.getX(), grav.getY(), grav.getZ()), timeToTakeOverMs);
	pfx->addAffector(out);
}

void ParticleSystem::addRotationAffector(const Vec3& rotSpd, const Vec3& pos) {
	if (!pfx) return;
	irr::scene::IParticleAffector* out = pfx->createRotationAffector(irr::core::vector3df(rotSpd.getX(), rotSpd.getY(), rotSpd.getZ()), irr::core::vector3df(pos.getX(), pos.getY(), pos.getZ()));
	pfx->addAffector(out);
}

void ParticleSystem::addScalarAffector(float scalar) {
	if (!pfx) return;
	irr::scene::IParticleAffector* out = pfx->createScaleParticleAffector(irr::core::dimension2df(scalar, scalar));
	pfx->addAffector(out);
}

void ParticleSystem::clearAffectors() {
	if (!pfx) return;
	pfx->removeAllAffectors();
}

void ParticleSystem::loadMaterial(const Material& mat) {
	if (!pfx) return;
	pfx->getMaterial(0) = mat.getMaterial();
}

void ParticleSystem::loadPointEmitter(const Vec3& dir) {
	if (!pfx) return;
	irr::core::vector3df direction(dir.getX(), dir.getY(), dir.getZ());
	irr::scene::IParticlePointEmitter* out = pfx->createPointEmitter(direction);
	pfx->setEmitter(out);
	out->drop();

	curType = 0;
	loadUserParams();
}

void ParticleSystem::loadBoxEmitter(const Vec3& dir, const Vec3& size) {
	if (!pfx) return;
	irr::core::vector3df direction(dir.getX(), dir.getY(), dir.getZ());
	irr::core::vector3df min(-1.0f * size.getX() / 2.0f, -1.0f * size.getY() / 2.0f, -1.0f * size.getY() / 2.0f);
	irr::core::vector3df max(size.getX() / 2.0f, size.getY() / 2.0f, size.getY() / 2.0f);
	irr::scene::IParticleBoxEmitter* out = pfx->createBoxEmitter(irr::core::aabbox3df(min, max), direction);
	pfx->setEmitter(out);
	out->drop();

	curType = 1;
	loadUserParams();
}

void ParticleSystem::loadSphereEmitter(const Vec3& dir, const Vec3& center, float radius) {
	if (!pfx) return;
	irr::core::vector3df direction(dir.getX(), dir.getY(), dir.getZ());
	irr::core::vector3df cent(center.getX(), center.getY(), center.getZ());
	irr::scene::IParticleSphereEmitter* out = pfx->createSphereEmitter(cent, radius, direction);
	pfx->setEmitter(out);
	out->drop();

	curType = 2;
	loadUserParams();
}

void ParticleSystem::loadRingEmitter(const Vec3& dir, const Vec3& center, float radius, float thickness) {
	if (!pfx) return;
	irr::core::vector3df direction(dir.getX(), dir.getY(), dir.getZ());
	irr::core::vector3df cent(center.getX(), center.getY(), center.getZ());
	irr::scene::IParticleRingEmitter* out = pfx->createRingEmitter(cent, radius, thickness, direction);
	pfx->setEmitter(out);
	out->drop();

	curType = 3;
	loadUserParams();
}

void ParticleSystem::loadCylinderEmitter(const Vec3& dir, const Vec3& center, float radius, float len, bool outlineOnly) {
	if (!pfx) return;
	irr::core::vector3df direction(dir.getX(), dir.getY(), dir.getZ());
	irr::core::vector3df cent(center.getX(), center.getY(), center.getZ());
	irr::scene::IParticleCylinderEmitter* out = pfx->createCylinderEmitter(cent, radius, irr::core::vector3df(0,1,0), len, outlineOnly, direction);
	pfx->setEmitter(out);
	out->drop();

	curType = 4;
	loadUserParams();
}

void ParticleSystem::loadUserParams() {
	if (!pfx) return;

	auto* e = pfx->getEmitter();
	if (!e) return;

	e->setMinLifeTime(minLife);
	e->setMaxLifeTime(maxLife);
	e->setMaxAngleDegrees(maxAng);
	e->setMinParticlesPerSecond(minRate);
	e->setMaxParticlesPerSecond(maxRate);
	e->setMinStartSize(irr::core::dimension2df(minScale, minScale));
	e->setMaxStartSize(irr::core::dimension2df(maxScale, maxScale));
	e->setMinStartColor(irr::video::SColor(minColor.a, minColor.r, minColor.g, minColor.b));
	e->setMaxStartColor(irr::video::SColor(maxColor.a, maxColor.r, maxColor.g, maxColor.b));
}

Vec2 ParticleSystem::getRates() const {
	if (!pfx) return Vec2();
	auto* e = pfx->getEmitter();
	if (!e) return Vec2();
	return Vec2(e->getMinParticlesPerSecond(), e->getMaxParticlesPerSecond());
}

void ParticleSystem::setRates(const Vec2& v) {
	if (!pfx) return;
	auto* e = pfx->getEmitter();
	if (!e) return;
	e->setMinLifeTime(v.getX());
	e->setMaxLifeTime(v.getY());

	minRate = v.getX();
	maxRate = v.getY();
}

int ParticleSystem::getMaxAngle() const {
	return pfx && pfx->getEmitter() ? pfx->getEmitter()->getMaxAngleDegrees() : 0;
}

void ParticleSystem::setMaxAngle(int v) {
	if (!pfx) return;
	auto* e = pfx->getEmitter();
	if (!e) return;
	e->setMaxAngleDegrees(v);
	maxAng = v;
}

Vec2 ParticleSystem::getMinMaxLife() const {
	if (!pfx) return Vec2();
	auto* e = pfx->getEmitter();
	if (!e) return Vec2();
	return Vec2(e->getMinLifeTime(), e->getMaxLifeTime());
}

void ParticleSystem::setMinMaxLife(const Vec2& v) {
	if (!pfx) return;
	auto* e = pfx->getEmitter();
	if (!e) return;
	e->setMinLifeTime(v.getX());
	e->setMaxLifeTime(v.getY());

	minLife = v.getX();
	maxLife = v.getY();
}

Vec2 ParticleSystem::getMinMaxScale() const {
	if (!pfx) return Vec2();
	auto* e = pfx->getEmitter();
	if (!e) return Vec2();
	return Vec2(e->getMinStartSize().Width, e->getMaxStartSize().Width);
}

void ParticleSystem::setMinMaxScale(const Vec2& v) {
	if (!pfx) return;
	auto* e = pfx->getEmitter();
	if (!e) return;
	e->setMinStartSize(irr::core::dimension2df(v.getX(), v.getX()));
	e->setMaxStartSize(irr::core::dimension2df(v.getY(), v.getY()));

	minScale = v.getX();
	maxScale = v.getY();
}

Vec4 ParticleSystem::getMinColor() const {
	if (!pfx) return Vec4();
	auto* e = pfx->getEmitter();
	if (!e) return Vec4();
	return Vec4(e->getMinStartColor().getRed(), e->getMinStartColor().getGreen(), e->getMinStartColor().getBlue(), e->getMinStartColor().getAlpha());
}

void ParticleSystem::setMinColor(const Vec4& v) {
	if (!pfx) return;
	auto* e = pfx->getEmitter();
	if (!e) return;
	e->setMinStartColor(irr::video::SColor(v.getW(), v.getX(), v.getY(), v.getZ()));

	minColor = Vec4S{ (int)v.getW(), (int)v.getX(), (int)v.getY(), (int)v.getZ() };
}

Vec4 ParticleSystem::getMaxColor() const {
	if (!pfx) return Vec4();
	auto* e = pfx->getEmitter();
	if (!e) return Vec4();
	return Vec4(e->getMaxStartColor().getRed(), e->getMaxStartColor().getGreen(), e->getMaxStartColor().getBlue(), e->getMaxStartColor().getAlpha());
}

void ParticleSystem::setMaxColor(const Vec4& v) {
	if (!pfx) return;
	auto* e = pfx->getEmitter();
	if (!e) return;
	e->setMaxStartColor(irr::video::SColor(v.getW(), v.getX(), v.getY(), v.getZ()));

	maxColor = Vec4S{ (int)v.getW(), (int)v.getX(), (int)v.getY(), (int)v.getZ() };
}

Vec3 ParticleSystem::getBoxSize() const {
	if (!pfx) return Vec3();
	auto* e = static_cast<irr::scene::IParticleBoxEmitter*>(pfx->getEmitter());
	if (!e) return Vec3();
	return Vec3(e->getBox().getExtent().X, e->getBox().getExtent().Y, e->getBox().getExtent().Z);

}

void ParticleSystem::setBoxSize(const Vec3& size) {
	if (!pfx) return;
	auto* e = static_cast<irr::scene::IParticleBoxEmitter*>(pfx->getEmitter());
	if (!e) return;
	irr::core::vector3df min(-1.0f * size.getX() / 2.0f, -1.0f * size.getY() / 2.0f, -1.0f * size.getY() / 2.0f);
	irr::core::vector3df max(size.getX() / 2.0f, size.getY() / 2.0f, size.getY() / 2.0f);
	e->setBox(irr::core::aabbox3df(min, max));
}

float ParticleSystem::getRadius() const {
	if (!pfx || !pfx->getEmitter()) return 0.0f;
	switch (pfx->getType()) {
	case irr::scene::EPET_SPHERE:
		return static_cast<irr::scene::IParticleSphereEmitter*>(pfx->getEmitter())->getRadius();
		break;
	case irr::scene::EPET_RING:
		return static_cast<irr::scene::IParticleRingEmitter*>(pfx->getEmitter())->getRadius();
		break;
	case irr::scene::EPET_CYLINDER:
		return static_cast<irr::scene::IParticleCylinderEmitter*>(pfx->getEmitter())->getRadius();
		break;
	}

	return 0.0f;
}

void ParticleSystem::setRadius(float v) {
	if (!pfx) return;
	switch (pfx->getType()) {
	case irr::scene::EPET_SPHERE:
		static_cast<irr::scene::IParticleSphereEmitter*>(pfx->getEmitter())->setRadius(v);
		break;
	case irr::scene::EPET_RING:
		static_cast<irr::scene::IParticleRingEmitter*>(pfx->getEmitter())->setRadius(v);
		break;
	case irr::scene::EPET_CYLINDER:
		static_cast<irr::scene::IParticleCylinderEmitter*>(pfx->getEmitter())->setRadius(v);
		break;
	}
}

float ParticleSystem::getCylinderLength() const {
	if (!pfx) return 0.0f;
	auto* e = static_cast<irr::scene::IParticleCylinderEmitter*>(pfx->getEmitter());
	if (!e) return 0.0f;
	return e->getLength();
}

void ParticleSystem::setCylinderLength(float v) {
	if (!pfx) return;
	auto* e = static_cast<irr::scene::IParticleCylinderEmitter*>(pfx->getEmitter());
	if (!e) return;
	e->setLength(v);
}

float ParticleSystem::getRingThickness() const {
	if (!pfx) return 0.0f;
	auto* e = static_cast<irr::scene::IParticleRingEmitter*>(pfx->getEmitter());
	if (!e) return 0.0f;
	return e->getRingThickness();
}

void ParticleSystem::setRingThickness(float v) {
	if (!pfx) return;
	auto* e = static_cast<irr::scene::IParticleRingEmitter*>(pfx->getEmitter());
	if (!e) return;
	e->setRingThickness(v);
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