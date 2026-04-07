#include "Modules/ModulePhysics.h"

#include "DebugConsole.h"
#include "Application.h"
#include "Renderer.h"
#include "PhysicsManager.h"

#include "Objects/Vec3.h"

#include <sol/sol.hpp>

static DebugConsole* d;
static Application* a;
static PhysicsManager* p;

void Module::Physics::bind(Application* app) {
	a = app;
	d = app->GetDebugConsole();
	p = app->GetRenderer()->GetPhysicsManager();
	sol::state& lua = app->GetLuaState();

	// Module Lime.Physics
	sol::table module = lua["Lime"]["Physics"].get_or_create<sol::table>();

	// [+] Returns the direction and magnitude of global gravity.
	// Returns Vec3
	module.set_function("getGravity", &Module::Physics::Bind::GetGravity);

	// [+] Sets the direction and magnitude of global gravity.
	// Params Vec3 gravity
	// Returns void
	module.set_function("setGravity", &Module::Physics::Bind::SetGravity);

	// [+] Returns whether or not the scene's physics simulation is paused.
	// Returns boolean
	module.set_function("isPaused", &Module::Physics::Bind::IsPaused);

	// [+] Sets whether or not the scene's physics simulation is paused.
	// Params boolean paused
	// Returns void
	module.set_function("setPaused", &Module::Physics::Bind::SetPaused);

	// [+] Returns the physics simulation step factor.
	// Returns number
	module.set_function("getStepFactor", &Module::Physics::Bind::GetStepFactor);

	// [+] Sets the physics simulation step factor.
	// Params number factor
	// Returns void
	module.set_function("setStepFactor", &Module::Physics::Bind::SetStepFactor);

	// [+] Sets the physics simulation debug mode.
	// Params Lime.Enum.PhysicsDebugType type
	// Returns void
	module.set_function("setDebug", &Module::Physics::Bind::SetDebugMode);

	// [+] Sets whether or not collisions with objects sharing the same ID should ignore one another.
	// Params boolean ignore
	// Returns void
	module.set_function("setIgnoreIdenticalID", &Module::Physics::Bind::SetIgnoreSameID);

	// End Module

	// Object CollisionResult, An object that stores collision data between two physics objects.
	// Field number depth, Penetration depth
	// Field Vec3 posA, Contact point on physics object A
	// Field Vec3 posB, Contact point on physics object B
	// Field Vec3 normal, Collision normal from physics object A to B
	// Field Vec3 linearVelocityA, Linear velocity of physics object A
	// Field Vec3 linearVelocityB, Linear velocity of physics object B
	// Field Vec3 angularVelocityA, Angular velocity of physics object A
	// Field Vec3 angularVelocityB, Angular velocity of physics object B
	// Field Vec3 velocityAtPointA, Actual velocity at physics object A contact point
	// Field Vec3 velocityAtPointB, Actual velocity at physics object B contact point
	// Field Vec3 relativeVelocity, velocityAtPointB - velocityAtPointA
	// Field number impactSpeed, Impact speed
	// Field table attributesB, Attributes of physics object B
	// End Object
}

// Functions

Vec3 Module::Physics::Bind::GetGravity() {
	return p->getGravity();
}

void Module::Physics::Bind::SetGravity(const Vec3& v) {
	p->setGravity(v);
}

bool Module::Physics::Bind::IsPaused() {
	return p->isPaused();
}

void Module::Physics::Bind::SetPaused(bool v) {
	p->setPaused(v);
}

float Module::Physics::Bind::GetStepFactor() {
	return p->getStepFactor();
}

void Module::Physics::Bind::SetStepFactor(float v) {
	p->setStepFactor(v);
}

void Module::Physics::Bind::SetDebugMode(int v) {
	p->setDebugMode(v);
}

void Module::Physics::Bind::SetIgnoreSameID(bool v) {
	p->setIgnoreSameID(v);
}