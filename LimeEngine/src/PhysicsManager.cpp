#include "PhysicsManager.h"
#include "DebugConsole.h"
#include "Application.h"
#include "Renderer.h"
#include "RenderHelper.h"
#include "Objects/Event.h"

#include "Objects/Mesh.h"
#include "Objects/Vec3.h"
#include "Interfaces/Object3D.h"
#include "irrBullet.h"
#include <sol/sol.hpp>

static Application* a = nullptr;
static Renderer* r = nullptr;
static DebugConsole* d = nullptr;
static irr::video::IVideoDriver* driver = nullptr;
static lua_State* l = nullptr;

struct ContactInfo {
	btScalar depth = 0.0f;
	btVector3 posA;
	btVector3 posB;
	btVector3 normalB;
	btVector3 linearVelA;
	btVector3 linearVelB;
	btVector3 angularVelA;
	btVector3 angularVelB;
	btVector3 velocityAtPointA;
	btVector3 velocityAtPointB;
	btVector3 relativeVelocity;
	btScalar normalSpeed = 0.0f;
	sol::table attributesA;
	sol::table attributesB;
};

struct ColliderInfo {
	irr::scene::ISceneNode* node;
	std::shared_ptr<Event> onEnter;
	std::shared_ptr<Event> onInside;
	std::shared_ptr<Event> onExit;
};

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
	handleCollisions();
	processCollisions();

	irr::video::SMaterial m;
	m.Lighting = false;
	driver->setMaterial(m);
	driver->setTransform(video::ETS_WORLD, core::matrix4());
	world->debugDrawWorld();
	// world->debugDrawProperties(true); // Prints to console too

	return true;
}

void PhysicsManager::SetLuaState(lua_State* ls) {
	l = ls;
}

bool PhysicsManager::guardPhysicsCheck() {
	if (!world) {
		d->Warn("Physics cannot be modified until the Lime window has been created!");
		return false;
	}
	return true;
}

bool PhysicsManager::addConstraintToWorld(btTypedConstraint* constraint, bool ignoreCollision) {
	if (!guardPhysicsCheck() || !constraint) return false;
	world->getPointer()->addConstraint(constraint, ignoreCollision);
}

bool PhysicsManager::removeConstraintFromWorld(btTypedConstraint* constraint) {
	if (!guardPhysicsCheck() || !constraint) return false;
	world->getPointer()->removeConstraint(constraint);
}

static btVector3 toBtVec3(Vec3 v) {
	return btVector3(v.getX(), v.getY(), v.getZ());
}

btHingeConstraint* PhysicsManager::createHingeConstraint(btRigidBody* a, btRigidBody* b, const Vec3& pivA, const Vec3& pivB, const Vec3& axisA, const Vec3& axisB) {
	if (!guardPhysicsCheck()) return nullptr;
	if (!a || !b) {
		d->Warn("Could not create HingeConstraint: Supplied RigidBody objects are invalid");
		return nullptr;
	}
	btHingeConstraint* out = new btHingeConstraint(*a, *b, toBtVec3(pivA), toBtVec3(pivB), toBtVec3(axisA), toBtVec3(axisB));
	return out;
}

btConeTwistConstraint* PhysicsManager::createConeTwistConstraint(btRigidBody* a, btRigidBody* b, btTransform ta, btTransform tb) {
	if (!guardPhysicsCheck()) return nullptr;
	if (!a || !b) {
		d->Warn("Could not create ConeTwistConstraint: Supplied RigidBody objects are invalid");
		return nullptr;
	}
	btConeTwistConstraint* out = new btConeTwistConstraint(*a, *b, ta, tb);
	return out;
}

IRigidBody* PhysicsManager::createRigidBody(const Object3D& m, const Mesh& c) {
	if (!guardPhysicsCheck()) return nullptr;

	irr::scene::ISceneNode* src = m.getNode();
	irr::scene::IAnimatedMeshSceneNode* collision = static_cast<irr::scene::IAnimatedMeshSceneNode*>(c.getNode());
	if (!src || !collision) return nullptr;

	auto shape = new IGImpactMeshShape(src, collision->getMesh(), 1.0f);
	IRigidBody* rb = world->addRigidBody(shape);
	rb->includeNodeOnRemoval(false);
	rb->setSleepingThresholds(0.5, 0.5);

	src->grab();
	collision->grab();
	
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

#include "Objects/RigidBody.h"
#include <sol/sol.hpp>
bool PhysicsManager::cleanRigidBodySource(irr::scene::IAnimatedMesh* src) {
	if (!world || !src) return false;

	RigidBody* rb = nullptr;
	bool col = false;
	auto s = rbMappedSrc.find(src);
	if (s != rbMappedSrc.end()) {
		rb = s->second;
		rbMappedSrc.erase(src);
	}

	auto c = rbMappedCol.find(src);
	if (c != rbMappedCol.end()) {
		if (!rb) { rb = c->second; col = true; }
		rbMappedCol.erase(src);
	}

	bool ok = rb != nullptr;
	if (ok) {
		rb->nullify();
		rb->destroy();
	}
	return ok;
}

void PhysicsManager::appendToMatchedRBSrc(irr::scene::IAnimatedMesh* src, RigidBody* rb) {
	rbMappedSrc[src] = rb;
}

void PhysicsManager::appendToMatchedRBCol(irr::scene::IAnimatedMesh* col, RigidBody* rb) {
	rbMappedCol[col] = rb;
}

void PhysicsManager::appendToCollisionDetection(btCollisionObject* col, const PhysicsObject& phys) {
	if (!col || !phys.getCollisionObject()) return;
	// colliderPair[col] = phys;
	// Follow how GUIManager stores a struct of events + other info!

	ColliderInfo out{ phys.getNode(), phys.onEnter, phys.onInside, phys.onExit };
	colliderPair[col] = out;
}

void PhysicsManager::removeFromCollisionDetection(btCollisionObject* col) {
	if (!col) return;
	colliderPair.erase(col);
}

void PhysicsManager::handleCollisions() {
	if (!world) return;

	btDispatcher* dispatcher = world->getPointer()->getDispatcher();
	if (!dispatcher) return;

	currentCollisions.clear();
	curData.clear();

	for (int i = 0; i < dispatcher->getNumManifolds(); ++i) {
		btPersistentManifold* manifold = dispatcher->getManifoldByIndexInternal(i);
		if (!manifold || manifold->getNumContacts() <= 0) continue;

		btCollisionObject* bodyA = const_cast<btCollisionObject*>(manifold->getBody0());
		btCollisionObject* bodyB = const_cast<btCollisionObject*>(manifold->getBody1());
		if (!bodyA || !bodyB) continue;

		auto mapA = colliderPair.find(bodyA);
		auto mapB = colliderPair.find(bodyB);
		if (mapA == colliderPair.end() || mapB == colliderPair.end()) continue;

		ColliderInfo infoA = mapA->second;
		ColliderInfo infoB = mapB->second;

		if (collisionsIgnoreSameID) {
			auto* nodeA = infoA.node;
			auto* nodeB = infoB.node;
			if (nodeA && nodeB && nodeA->getID() == nodeB->getID())
				continue;
		}

		const btManifoldPoint* closest = nullptr;
		btScalar lowestDist = BT_LARGE_FLOAT;

		for (int j = 0; j < manifold->getNumContacts(); ++j) {
			btManifoldPoint& pt = manifold->getContactPoint(j);

			if (pt.getDistance() <= 0.0f && pt.getDistance() < lowestDist) {
				lowestDist = pt.getDistance();
				closest = &pt;
			}
		}

		if (!closest) continue;
		if (bodyA > bodyB) std::swap(bodyA, bodyB);

		std::pair<btCollisionObject*, btCollisionObject*> pair = { bodyA, bodyB };
		currentCollisions.insert(pair);

		ContactInfo info;
		info.depth = lowestDist;
		info.posA = closest->getPositionWorldOnA();
		info.posB = closest->getPositionWorldOnB();
		info.normalB = closest->m_normalWorldOnB;

		// Change when adding softbodies
		btRigidBody* bulletA = btRigidBody::upcast(bodyA);
		btRigidBody* bulletB = btRigidBody::upcast(bodyB);

		if (bulletA) {
			info.linearVelA = bulletA->getLinearVelocity();
			info.angularVelA = bulletA->getAngularVelocity();
			btVector3 relPosA = info.posA - bulletA->getWorldTransform().getOrigin();
			info.velocityAtPointA = bulletA->getVelocityInLocalPoint(relPosA);
		}

		if (bulletB) {
			info.linearVelB = bulletB->getLinearVelocity();
			info.angularVelB = bulletB->getAngularVelocity();
			btVector3 relPosB = info.posB - bulletB->getWorldTransform().getOrigin();
			info.velocityAtPointB = bulletB->getVelocityInLocalPoint(relPosB);
		}

		info.relativeVelocity = info.velocityAtPointB - info.velocityAtPointA;
		info.normalSpeed = info.relativeVelocity.dot(info.normalB);

		curData[pair] = info;
	}
}

static Vec3 toVec3(btVector3 v) {
	return Vec3(v.getX(), v.getY(), v.getZ());
}

static sol::table getResult(ContactInfo info, bool isB) {
	sol::state_view view(l);
	sol::table out = view.create_table();

	out["depth"] = info.depth;
	out["posA"] = !isB ? toVec3(info.posA) : toVec3(info.posB);
	out["posB"] = isB ? toVec3(info.posA) : toVec3(info.posB);
	out["normal"] = toVec3(info.normalB);
	out["linearVelocityA"] = !isB ? toVec3(info.linearVelA) : toVec3(info.linearVelB);
	out["linearVelocityB"] = isB ? toVec3(info.linearVelA) : toVec3(info.linearVelB);
	out["angularVelocityA"] = !isB ? toVec3(info.angularVelA) : toVec3(info.angularVelB);
	out["angularVelocityB"] = isB ? toVec3(info.angularVelA) : toVec3(info.angularVelB);
	out["velocityAtPointA"] = !isB ? toVec3(info.velocityAtPointA) : toVec3(info.velocityAtPointB);
	out["velocityAtPointB"] = isB ? toVec3(info.velocityAtPointA) : toVec3(info.velocityAtPointB);
	out["relativeVelocity"] = toVec3(info.relativeVelocity);
	out["impactSpeed"] = (0.0 > -info.normalSpeed) ? 0.0 : -info.normalSpeed;
	out["attributesB"] = isB ? info.attributesA : info.attributesB;

	return out;
}

void PhysicsManager::processCollisions() {
	// Enter, Inside
	RenderHelper* rh = r->GetRenderHelper();

	for (const auto& pair : currentCollisions) {
		auto aIt = colliderPair.find(pair.first);
		auto bIt = colliderPair.find(pair.second);
		if (aIt == colliderPair.end() || bIt == colliderPair.end()) continue;

		ColliderInfo physA = aIt->second;
		ColliderInfo physB = bIt->second;
		if (!physA.node || !physB.node) continue;

		auto infoIt = curData.find(pair);
		if (infoIt == curData.end()) continue;

		ContactInfo& info = infoIt->second;
		sol::state_view view(l);
		info.attributesA = rh->getAttributes(physA.node);
		info.attributesB = view.create_table(physB.node);

		if (!lastCollisions.count(pair)) {
			// Call Enter
			physA.onEnter.get()->engineRun([&](const std::string& msg) { d->PostError(msg); }, getResult(info, false));
			physB.onEnter.get()->engineRun([&](const std::string& msg) { d->PostError(msg); }, getResult(info, true));
		} else {
			// Call Inside
			physA.onInside.get()->engineRun([&](const std::string& msg) { d->PostError(msg); }, getResult(info, false));
			physB.onInside.get()->engineRun([&](const std::string& msg) { d->PostError(msg); }, getResult(info, true));
		}
	}

	// Exit
	for (const auto& pair : lastCollisions) {
		if (currentCollisions.count(pair)) continue;

		auto aIt = colliderPair.find(pair.first);
		auto bIt = colliderPair.find(pair.second);
		if (aIt == colliderPair.end() || bIt == colliderPair.end()) continue;

		ColliderInfo physA = aIt->second;
		ColliderInfo physB = bIt->second;
		if (!physA.node || !physB.node) continue;

		// Run Exit, doesn't have info
		physA.onExit.get()->engineRun([&](const std::string& msg) { d->PostError(msg); });
		physB.onExit.get()->engineRun([&](const std::string& msg) { d->PostError(msg); });
	}

	lastCollisions = currentCollisions;
}
