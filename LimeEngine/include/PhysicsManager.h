#pragma once
#include <string>
#include <set>

class DebugConsole;
class Renderer;

// irrb
class irrBulletWorld;
class btCollisionObject;
class ContactInfo;
class Vec3;
class PhysicsObject; // implement

namespace irr {
	class IrrlichtDevice;
}

class PhysicsManager {
public:
	PhysicsManager(Renderer* owner, DebugConsole* debug);
	~PhysicsManager() = default;

	bool Init(irr::IrrlichtDevice* device);
	bool Update(float dt);

	bool guardPhysicsCheck();

	Vec3 getGravity() const;
	void setGravity(const Vec3& grav);

	bool isPaused() const;
	void setPaused(bool v);

	float getStepFactor() const { return stepFactor; }
	void setStepFactor(float v) { stepFactor = v; } // Multiplier to dt

	void setDebugMode(int v);

	// Callbacks
	void handleCollisions();
	std::set<std::pair<btCollisionObject*, btCollisionObject*>> lastCollisions;
	std::set<std::pair<btCollisionObject*, btCollisionObject*>> currentCollisions;
	std::unordered_map<btCollisionObject*, ContactInfo> curData;
	std::unordered_map<btCollisionObject*, PhysicsObject*> colliderPair;

	void setIgnoreSameID(bool v) { collisionsIgnoreSameID = v; }
private:
	irrBulletWorld* world = nullptr;

	float stepFactor = 1.0f;
	bool collisionsIgnoreSameID = false;
};