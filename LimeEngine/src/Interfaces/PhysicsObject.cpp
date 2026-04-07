#include "Interfaces/PhysicsObject.h"
#include "PhysicsManager.h"
#include "Objects/Event.h"
#include <sol/sol.hpp>

#include "irrBullet.h"

static lua_State* l;
static PhysicsManager* p;

struct ContactInfo {
	btScalar depth;
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
	btScalar normalSpeed;
};

void PhysicsObject::createCallbacks() {
	onEnter = std::make_shared<Event>();
	onInside = std::make_shared<Event>();
	onExit = std::make_shared<Event>();
}

void PhysicsObject::setCollisionInfo(ContactInfo info, bool isB) {
	// Convert info to sol table, isB flips
}

sol::table PhysicsObject::getCollision() {
	sol::state_view view(l);
	return getNode() ? curCollisionInfo : view.create_table();
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

    // End Interface
}