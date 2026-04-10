#pragma once
#include <sol/optional.hpp>
#include <sol/forward.hpp>

extern "C" { struct lua_State; }

// irrb
class IRigidBody;
class btCollisionObject;
class btTypedConstraint;
class btHingeConstraint;
class btConeTwistConstraint;

class PhysicsManager;
class RigidBody;
class Vec2;
class Vec3;

// Constraint interface
class Constraint {
public:
	virtual ~Constraint() = default;

	bool addToWorld();
	bool removeFromWorld();

	bool getEnabled() const;
	void setEnabled(bool v);

	float getBreakingImpulse() const;
	void setBreakingImpulse(float impulse);

	bool getIgnoreSelves() const { return ignoreSelves; }
	void setIgnoreSelves(bool v) { ignoreSelves = v; }

	sol::object destroy();

	virtual btTypedConstraint* getConstraint() const = 0;
private:
	bool inWorld = false;
	bool ignoreSelves = false;
};

// Hinge
class HingeConstraint : public Constraint {
public:
	HingeConstraint(const RigidBody& rA, const RigidBody& rB, const Vec3& pivA, const Vec3& pivB, const Vec3& axisA, const Vec3& axisB);

	Vec2 getLimit() const;
	void setLimit(const Vec2& v);
	
	bool getMotor() const;
	void setMotor(bool v);

	float getMotorVelocity() const;
	void setMotorVelocity(float v);

	float getMaxMotorImpulse() const;
	void setMaxMotorImpulse(float v);

	btTypedConstraint* getConstraint() const override;
private:
	btHingeConstraint* constraint = nullptr;

	bool motor = false;
	float motorVel = 0.0f;
	float maxMotor = 0.0f;
};

// ConeTwist
class ConeTwistConstraint : public Constraint {
public:
	ConeTwistConstraint(const RigidBody& rA, const RigidBody& rB, const Vec3& pivA, const Vec3& pivB, const Vec3& axisA, const Vec3& axisB);

	Vec2 getSwingLimit() const;
	void setSwingLimit(const Vec2& v);

	float getTwistLimit() const;
	void setTwistLimit(float v);

	btTypedConstraint* getConstraint() const override;
private:
	btConeTwistConstraint* constraint = nullptr;

	float swing1 = 45.0f;
	float swing2 = 45.0f;
	float twist = 25.0f;
};

// Binders
namespace Interface::ConstraintBind {
	void bind(lua_State* ls, PhysicsManager* ps);
}

namespace Object {
	namespace HingeConstraintBind {
		void bind(lua_State* ls);
	}
	namespace ConeTwistConstraintBind {
		void bind(lua_State* ls);
	}
}