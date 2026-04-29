#include "Objects/Constraints.h"

#include "PhysicsManager.h"
#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/RigidBody.h"
#include <sol/sol.hpp>
#include <irrBullet.h>
#include <cmath>

static lua_State* l = nullptr;
static PhysicsManager* p = nullptr;

static btVector3 toBtVec3(Vec3 v) {
	return btVector3(v.getX(), v.getY(), v.getZ());
}

// Constraint interface

bool Constraint::addToWorld() {
	if (!getConstraint()) return false;
	if (!getConstraint()->getRigidBodyA().getUserPointer() || !getConstraint()->getRigidBodyB().getUserPointer()) return false;
	bool ok = p->addConstraintToWorld(getConstraint(), ignoreSelves);
	if (ok) inWorld = true;
	return ok;
}

bool Constraint::removeFromWorld() {
	inWorld = false;
	return p->removeConstraintFromWorld(getConstraint());
}

bool Constraint::getEnabled() const {
	return getConstraint() && inWorld ? getConstraint()->isEnabled() : false;
}

void Constraint::setEnabled(bool v) {
	if (!getConstraint()) return;
	getConstraint()->setEnabled(v);

	if (!inWorld && v) addToWorld();
	else if (inWorld && !v) removeFromWorld();
}

float Constraint::getBreakingImpulse() const {
	return getConstraint() ? getConstraint()->getBreakingImpulseThreshold() : 0.0f;
}

void Constraint::setBreakingImpulse(float v) {
	if (!getConstraint()) return;
	getConstraint()->setBreakingImpulseThreshold(v);
}

sol::object Constraint::destroy() {
	if (getConstraint() && inWorld) removeFromWorld();
	if (getConstraint()) delete getConstraint();
	return sol::make_object(l, sol::nil);
}

void Interface::ConstraintBind::bind(lua_State* ls, PhysicsManager* ps) {
	l = ls;
	p = ps;
	sol::state_view view(ls);

	// Interface Constraint

	sol::usertype<Constraint> obj = view.new_usertype<Constraint>(
		"Constraint",

		// Field boolean active, Whether or not this `Constraint` is active.
		"active", sol::property(&Constraint::getEnabled, &Constraint::setEnabled),
		// Field boolean ignoreCollision, Whether or not the `RigidBody` objects of this `Constraint` should ignore collision between one another. This value should be altered prior to activating this `Constraint` to take effect in the scene.
		"ignoreCollision", sol::property(&Constraint::getIgnoreSelves, &Constraint::setIgnoreSelves),
		// Field number breakThreshold, The impulse threshold this `Constraint` can endure before it breaks, deactivating itself. Physics objects default to unbreakable, but altering this value will enable this object to be prone to breaking.
		"breakThreshold", sol::property(&Constraint::getBreakingImpulse, &Constraint::setBreakingImpulse)
	);

	// Destroys this `Constraint`.
	// Returns nil
	obj.set_function("destroy", &Constraint::destroy);

	// End Interface
}

// Hinge

HingeConstraint::HingeConstraint(const RigidBody& rA, const RigidBody& rB, const Vec3& pivA, const Vec3& pivB, const Vec3& axisA, const Vec3& axisB) {
	btRigidBody* bA = rA.getCollisionObject() ? btRigidBody::upcast(rA.getCollisionObject()) : nullptr;
	btRigidBody* bB = rB.getCollisionObject() ? btRigidBody::upcast(rB.getCollisionObject()) : nullptr;

	if (!bA || !bB) {
		bA = nullptr;
		bB = nullptr;
	}

	constraint = p->createHingeConstraint(bA, bB, pivA, pivB, axisA, axisB);
}

Vec2 HingeConstraint::getLimit() const {
	return constraint ? Vec2(constraint->getLowerLimit() * 180.0 / M_PI, constraint->getUpperLimit() * 180.0 / M_PI) : Vec2();
}

void HingeConstraint::setLimit(const Vec2& v) {
	if (!constraint) return;
	constraint->setLimit(v.getX() * M_PI / 180.0, v.getY() * M_PI / 180.0);
}

bool HingeConstraint::getMotor() const {
	return constraint ? motor : false;
}

void HingeConstraint::setMotor(bool v) {
	if (!constraint) return;
	motor = v;
	constraint->enableMotor(v);
}

float HingeConstraint::getMotorVelocity() const {
	return constraint ? motorVel : 0.0f;
}

void HingeConstraint::setMotorVelocity(float v) {
	if (!constraint) return;
	motorVel = v;
	constraint->setMotorTargetVelocity(v);
}

float HingeConstraint::getMaxMotorImpulse() const {
	return constraint ? maxMotor : 0.0f;
}

void HingeConstraint::setMaxMotorImpulse(float v) {
	if (!constraint) return;
	maxMotor = v;
	constraint->setMaxMotorImpulse(v);
}

btTypedConstraint* HingeConstraint::getConstraint() const {
	return constraint;
}

void Object::HingeConstraintBind::bind(lua_State* ls) {

	// Object HingeConstraint, A physics `Constraint` that hinges two `RigidBody` objects together. Pivots and axis parameters are in local space. Axis vector values range from 0 to 1, where 1 allows rotation around said axis and vice versa.
	// Inherits Constraint

	// Constructor RigidBody rbA, RigidBody rbB, Vec3 localPivotA, Vec3 localPivotB, Vec3 localAxisA, Vec3 localAxisB

	sol::state_view view(ls);
	sol::usertype<HingeConstraint> obj = view.new_usertype<HingeConstraint>(
		"HingeConstraint",
		sol::constructors<HingeConstraint(const RigidBody& rA, const RigidBody& rB, const Vec3& pivA, const Vec3& pivB, const Vec3& axisA, const Vec3& axisB)>(),

		sol::base_classes, sol::bases<Constraint>(),
		sol::meta_function::type, [](const HingeConstraint&) { return "HingeConstraint"; },

		// Field Vec2 limits, The angular limits of the hinge, where `x` is lower limit and `y` is upper limit.
		"limits", sol::property(
			[](HingeConstraint& c) { return Vec2( [&c]{ return c.getLimit(); }, [&c](const Vec2& v){ c.setLimit(v); } ); },
			[](HingeConstraint& c, const Vec2& v) { c.setLimit(v); }
		),

		// Field boolean motor, Whether or not this `Constraint` applies angular velocity every physics step, rotating itself like a motor.
		"motor", sol::property(&HingeConstraint::getMotor, &HingeConstraint::setMotor),

		// Field number motorVelocity, If this `Constraint` is a motor, this determines the target rotational velocity.
		"motorVelocity", sol::property(&HingeConstraint::getMotorVelocity, &HingeConstraint::setMotorVelocity),

		// Field number maxMotorImpulse, If this `Constraint` is a motor, this determines the maximum force allowed to spin to reach the target motorVelocity.
		"maxMotorImpulse", sol::property(&HingeConstraint::getMaxMotorImpulse, &HingeConstraint::setMaxMotorImpulse)
	);

	obj[sol::meta_function::to_string] = [](const HingeConstraint& v) {
		return "HingeConstraint";
		};

	// End Object
}

// Cone Twist

static btMatrix3x3 makeBasisFromAxis(const Vec3& axis) {
	btVector3 z(axis.getX(), axis.getY(), axis.getZ());
	if (z.length2() < SIMD_EPSILON) z = btVector3(0, 1, 0);
	z.normalize();

	btVector3 up = (btFabs(z.dot(btVector3(0, 1, 0))) > 0.999f) ? btVector3(1, 0, 0) : btVector3(0, 1, 0);

	btVector3 x = up.cross(z);
	if (x.length2() < SIMD_EPSILON) x = btVector3(1, 0, 0);
	x.normalize();

	btVector3 y = z.cross(x);
	y.normalize();

	return btMatrix3x3(
		x.getX(), y.getX(), z.getX(),
		x.getY(), y.getY(), z.getY(),
		x.getZ(), y.getZ(), z.getZ()
	);
}

ConeTwistConstraint::ConeTwistConstraint(const RigidBody& rA, const RigidBody& rB, const Vec3& pivA, const Vec3& pivB, const Vec3& axisA, const Vec3& axisB) {
	btRigidBody* bA = rA.getCollisionObject() ? btRigidBody::upcast(rA.getCollisionObject()) : nullptr;
	btRigidBody* bB = rB.getCollisionObject() ? btRigidBody::upcast(rB.getCollisionObject()) : nullptr;

	if (!bA || !bB) {
		bA = nullptr;
		bB = nullptr;
	}

	btTransform frameA;
	btTransform frameB;
	frameA.setIdentity();
	frameB.setIdentity();

	frameA.setOrigin(toBtVec3(pivA));
	frameB.setOrigin(toBtVec3(pivB));

	frameA.setBasis(makeBasisFromAxis(axisA));
	frameB.setBasis(makeBasisFromAxis(axisB));

	constraint = p->createConeTwistConstraint(bA, bB, frameA, frameB);
	if (constraint)
		constraint->setLimit(
			swing1 * SIMD_RADS_PER_DEG,
			swing2 * SIMD_RADS_PER_DEG,
			twist * SIMD_RADS_PER_DEG
		);
}

Vec2 ConeTwistConstraint::getSwingLimit() const {
	return constraint ? Vec2(swing1, swing2) : Vec2();
}

void ConeTwistConstraint::setSwingLimit(const Vec2& v) {
	swing1 = v.getX();
	swing2 = v.getY();

	if (!constraint) return;
	constraint->setLimit(
		swing1 * SIMD_RADS_PER_DEG,
		swing2 * SIMD_RADS_PER_DEG,
		twist * SIMD_RADS_PER_DEG
	);
}

float ConeTwistConstraint::getTwistLimit() const {
	return constraint ? twist : 0.0f;
}

void ConeTwistConstraint::setTwistLimit(float v) {
	twist = v;

	if (!constraint) return;
	constraint->setLimit(
		swing1 * SIMD_RADS_PER_DEG,
		swing2 * SIMD_RADS_PER_DEG,
		twist * SIMD_RADS_PER_DEG
	);
}

btTypedConstraint* ConeTwistConstraint::getConstraint() const {
	return constraint;
}

void Object::ConeTwistConstraintBind::bind(lua_State* ls) {

	// Object ConeTwistConstraint, A physics `Constraint` that twists two `RigidBody` objects together like a limited socket joint. Pivots and axis parameters are in local space. Axis vector values range from 0 to 1, where 1 allows rotation around said axis and vice versa.
	// Inherits Constraint

	// Constructor RigidBody rbA, RigidBody rbB, Vec3 localPivotA, Vec3 localPivotB, Vec3 localAxisA, Vec3 localAxisB

	sol::state_view view(ls);
	sol::usertype<ConeTwistConstraint> obj = view.new_usertype<ConeTwistConstraint>(
		"ConeTwistConstraint",
		sol::constructors<ConeTwistConstraint(const RigidBody& rA, const RigidBody& rB, const Vec3& pivA, const Vec3& pivB, const Vec3& axisA, const Vec3& axisB)>(),

		sol::base_classes, sol::bases<Constraint>(),
		sol::meta_function::type, [](const ConeTwistConstraint&) { return "ConeTwistConstraint"; },
	
		// Field Vec2 swingLimits, The angular swing limits of this `Constraint`, where `x` is sideways and `y` is forward and backward.
		"swingLimits", sol::property(
			[](ConeTwistConstraint& c) { return Vec2( [&c]{ return c.getSwingLimit(); }, [&c](const Vec2& v){ c.setSwingLimit(v); } ); },
			[](ConeTwistConstraint& c, const Vec2& v) { c.setSwingLimit(v); }
		),

		// Field number twistLimit, The angular twist limit.
		"twistLimit", sol::property(&ConeTwistConstraint::getTwistLimit, &ConeTwistConstraint::setTwistLimit)
	);

	obj[sol::meta_function::to_string] = [](const ConeTwistConstraint& v) {
		return "ConeTwistConstraint";
		};

	// End Object
}