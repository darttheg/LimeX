#include "PhysicsManager.h"
#include "DebugConsole.h"
#include "Renderer.h"

#include "Objects/Mesh.h"
#include "Objects/Vec3.h"
#include "irrBullet.h"

static Renderer* r = nullptr;
static DebugConsole* d = nullptr;
static irr::video::IVideoDriver* driver = nullptr;

PhysicsManager::PhysicsManager(Renderer* owner, DebugConsole* debug) {
	r = owner;
	d = debug;
}

bool PhysicsManager::Init(irr::IrrlichtDevice* device) {
	if (!device) return false;
	world = createIrrBulletWorld(device, true, true);
	if (!world) return false;

	world->setGravity(irr::core::vector3df(0, -9.8, 0));

	driver = device->getVideoDriver();

	return true;
}

bool PhysicsManager::Update(float dt) {
	if (!world) return false;

	const btScalar fixed = 1.0f / 60.0f;
	const int maxSubSteps = 8;
	int out = world->stepSimulation(dt * stepFactor, maxSubSteps, fixed);

	irr::video::SMaterial m;
	m.Lighting = false;
	driver->setMaterial(m);
	driver->setTransform(video::ETS_WORLD, core::matrix4());
	world->debugDrawWorld();
	// world->debugDrawProperties(true); // Prints to console too

	return true;
}

bool PhysicsManager::guardPhysicsCheck() {
	if (!world) {
		d->Warn("Physics cannot be modified until the Lime window has been created!");
		return false;
	}
	return true;
}

IRigidBody* PhysicsManager::createRigidBody(const Mesh& m, const Mesh& c) {
	if (!guardPhysicsCheck()) return nullptr;

	irr::scene::IAnimatedMeshSceneNode* mesh = static_cast<irr::scene::IAnimatedMeshSceneNode*>(m.getNode());
	irr::scene::IAnimatedMeshSceneNode* collision = static_cast<irr::scene::IAnimatedMeshSceneNode*>(c.getNode());
	if (!mesh || !collision) return nullptr;

	auto shape = new IGImpactMeshShape(mesh, collision->getMesh(), 1.0f);
	IRigidBody* rb = world->addRigidBody(shape);
	rb->includeNodeOnRemoval(false);
	rb->setSleepingThresholds(0.1, 0.1);
	
	return rb;
}

void PhysicsManager::removeRigidBody(IRigidBody* rb) {
	if (!guardPhysicsCheck()) return;
	world->removeCollisionObject(rb, true);
}

Vec3 PhysicsManager::getGravity() {
	if (!guardPhysicsCheck()) return Vec3();
	btVector3 grav = world->getPointer()->getGravity();
	return Vec3(grav.getX(), grav.getY(), grav.getZ());
}

void PhysicsManager::setGravity(const Vec3& grav) {
	if (!guardPhysicsCheck()) return;
	world->setGravity(irr::core::vector3df(grav.getX(), grav.getY(), grav.getZ()));
}

bool PhysicsManager::isPaused() {
	return guardPhysicsCheck() ? world->simulationPaused() : false;
}

void PhysicsManager::setPaused(bool v) {
	if (!guardPhysicsCheck()) return;
	world->pauseSimulation(v);
}

void PhysicsManager::setDebugMode(int v) {
	if (!guardPhysicsCheck()) return;

	irrPhysicsDebugMode out = irrPhysicsDebugMode::EPDM_NoDebug;
	switch (v) {
	case 1:
		out = irrPhysicsDebugMode::EPDM_DrawAabb;
		break;
	case 2:
		out = irrPhysicsDebugMode::EPDM_DrawContactPoints;
		break;
	case 3:
		out = irrPhysicsDebugMode::EPDM_DrawWireframe;
		break;
	case 4:
		out = irrPhysicsDebugMode::EPDM_DrawConstraints | irrPhysicsDebugMode::EPDM_DrawConstraintLimits;
		break;
	case 5:
		out = irrPhysicsDebugMode::EPDM_DrawAabb | irrPhysicsDebugMode::EPDM_DrawContactPoints | irrPhysicsDebugMode::EPDM_DrawWireframe |
			irrPhysicsDebugMode::EPDM_DrawConstraints | irrPhysicsDebugMode::EPDM_DrawConstraintLimits;
		break;
	default:
		out = irrPhysicsDebugMode::EPDM_NoDebug;
	}

	world->setDebugMode(out);
}