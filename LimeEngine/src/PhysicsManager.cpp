#include "PhysicsManager.h"
#include "DebugConsole.h"
#include "Renderer.h"

#include "Objects/Vec3.h"
#include "irrBullet.h"

static Renderer* r = nullptr;
static DebugConsole* d = nullptr;

PhysicsManager::PhysicsManager(Renderer* owner, DebugConsole* debug) {
	r = owner;
	d = debug;
}

bool PhysicsManager::Init(irr::IrrlichtDevice* device) {
	world = createIrrBulletWorld(device, true, true);
	if (!world) return false;

	world->setGravity(irr::core::vector3df(0, -9.8, 0));

	return true;
}

bool PhysicsManager::guardPhysicsCheck() {
	if (!world) {
		d->Warn("Physics cannot be modified until the Lime window has been created!");
		return false;
	}
	return true;
}

Vec3 PhysicsManager::getGravity() const {
	if (!world) return Vec3();
	btVector3 grav = world->getPointer()->getGravity();
	return Vec3(grav.getX(), grav.getY(), grav.getZ());
}

void PhysicsManager::setGravity(const Vec3& grav) {
	if (!guardPhysicsCheck()) return;
	world->setGravity(irr::core::vector3df(grav.getX(), grav.getY(), grav.getZ()));
}

bool PhysicsManager::isPaused() const {
	return world ? world->simulationPaused() : false;
}

void PhysicsManager::setPaused(bool v) {
	if (!guardPhysicsCheck()) return;
	world->pauseSimulation(v);
}