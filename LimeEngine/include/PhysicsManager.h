#pragma once
#include <string>
#include <unordered_map>
#include <set>

class DebugConsole;
class Renderer;

// irrb
class irrBulletWorld;
class btCollisionObject;
class ContactInfo;
class Vec3;
class PhysicsObject;
class IRigidBody;
class Mesh;
class Object3D;

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

	// Create
	IRigidBody* createRigidBody(const Object3D& m, const Mesh& c);

	// Clean-up
	void removeRigidBody(IRigidBody* rb);

	// Configuration
	Vec3 getGravity();
	void setGravity(const Vec3& grav);
	bool isPaused();
	void setPaused(bool v);
	float getStepFactor() { return stepFactor; }
	void setStepFactor(float v) { stepFactor = v; } // Multiplier to dt
	void setDebugMode(int v);
	void setIgnoreSameID(bool v) { collisionsIgnoreSameID = v; }
private:
	irrBulletWorld* world = nullptr;

	float stepFactor = 1.0f;
	bool collisionsIgnoreSameID = false;

	// Callbacks
	/*void handleCollisions();
	std::set<std::pair<btCollisionObject*, btCollisionObject*>> lastCollisions;
	std::set<std::pair<btCollisionObject*, btCollisionObject*>> currentCollisions;
	std::unordered_map<btCollisionObject*, ContactInfo> curData;
	std::unordered_map<btCollisionObject*, PhysicsObject*> colliderPair;*/
};