#include "Interfaces/PhysicsObject.h"
#include "PhysicsManager.h"
#include "Objects/Event.h"
#include <sol/sol.hpp>

#include "irrBullet.h"

static lua_State* l;
static PhysicsManager* p;

void PhysicsObject::createCallbacks() {
	onEnter = std::make_shared<Event>();
	onInside = std::make_shared<Event>();
	onExit = std::make_shared<Event>();
}

void PhysicsObject::insertIntoCallbacks() {
	p->appendToCollisionDetection(getCollisionObject(), *this);
}

void PhysicsObject::removeFromCallbacks() {
	p->removeFromCollisionDetection(getCollisionObject());
}

PhysicsObject::PhysicsObject() {
	if (!p->guardPhysicsCheck("Cannot create physics object prior to window creation.")) return;
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

    // Field Event onEnter, Event called when another physics object collides with this object for the first time.
	// Params CollisionResult result
	obj["onEnter"] = sol::property([](PhysicsObject& self) { return self.getEnterEvent(); });
    // Field Event onInside, Event called when another physics object is inside this physics object.
	obj["onInside"] = sol::property([](PhysicsObject& self) { return self.getInsideEvent(); });
    // Field Event onExit, Event called when another physics object exits this physics object.
	obj["onExit"] = sol::property([](PhysicsObject& self) { return self.getExitEvent(); });

	// Destroys this object.
	// Returns nil
	obj.set_function("destroy", &PhysicsObject::i_destroy);

    // End Interface
}