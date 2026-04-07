#include "Interfaces/PhysicsObject.h"
#include "PhysicsManager.h"
#include "Objects/Event.h"
#include <sol/sol.hpp>

#include "irrBullet.h"

static lua_State* l;
static PhysicsManager* p;

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

void PhysicsObject::createCallbacks() {
	onEnter = std::make_shared<Event>();
	onInside = std::make_shared<Event>();
	onExit = std::make_shared<Event>();
	
	sol::state_view view(l);
	colInfo = view.create_table();
}

void PhysicsObject::insertIntoCallbacks() {
	p->appendToCollisionDetection(getCollisionObject(), *this);
}

void PhysicsObject::removeFromCallbacks() {
	p->removeFromCollisionDetection(getCollisionObject());
}

#include "Objects/Vec3.h"
static Vec3 toVec3(btVector3 v) {
	return Vec3(v.getX(), v.getY(), v.getZ());
}

void PhysicsObject::setCollisionInfo(ContactInfo info, bool isB) {
	// Convert info to sol table, isB flips
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

	colInfo = out;
}

sol::table PhysicsObject::getCollision() {
	sol::state_view view(l);
	return (getNode() && colInfo) ? colInfo : view.create_table();
}

sol::object PhysicsObject::i_destroy() {
	removeFromCallbacks();
	destroy();
	return sol::make_object(l, sol::nil);
}

void Interface::PhysicsObjectBind::bind(lua_State* ls, PhysicsManager* ps) {
    l = ls;
	p = ps;
    sol::state_view view(ls);

    // Interface PhysicsObject

    sol::usertype<PhysicsObject> obj = view.new_usertype<PhysicsObject>(
        "PhysicsObject"
    );

	// Returns a `CollisionResult` object containing data about the current collision `Event`.
	// Returns CollisionResult
	obj.set_function("getCollisionResult", &PhysicsObject::getCollision);

    // Field Event onEnter, Event called when another physics object collides with this object for the first time. Use `:getCollisionResult()` to retrieve collision data.
	obj["onEnter"] = sol::property([](PhysicsObject& self) { return self.getEnterEvent(); });
    // Field Event onInside, Event called when another physics object is inside this physics object. Use `:getCollisionResult()` to retrieve collision data.
	obj["onInside"] = sol::property([](PhysicsObject& self) { return self.getInsideEvent(); });
    // Field Event onExit, Event called when another physics object exits this physics object.
	obj["onExit"] = sol::property([](PhysicsObject& self) { return self.getExitEvent(); });

	// Destroys this object.
	// Returns nil
	obj.set_function("destroy", &PhysicsObject::i_destroy);

    // End Interface
}