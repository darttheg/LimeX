#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <sol/forward.hpp>

class DebugConsole;
class Renderer;

// irrb
class irrBulletWorld;
class btCollisionObject;
class ContactInfo;
class Vec3;
class PhysicsObject;
class IRigidBody;
class btRigidBody;
class RigidBody;
class Mesh;
class Object3D;
class btTypedConstraint;
class btHingeConstraint;
class btConeTwistConstraint;
class btTransform;

namespace irr {
	class IrrlichtDevice;
	namespace scene {
		class ISceneNode;
		class IAnimatedMesh;
	}
}

struct ContactInfo;
struct ColliderInfo;

struct PairHash {
	size_t operator()(const std::pair<btCollisionObject*, btCollisionObject*>& p) const {
		return std::hash<uintptr_t>()((uintptr_t)p.first) ^ (std::hash<uintptr_t>()((uintptr_t)p.second) << 32);
	}
};

class PhysicsManager {
public:
	PhysicsManager(Renderer* owner, DebugConsole* debug);
	~PhysicsManager() = default;

	bool Init(irr::IrrlichtDevice* device);
	bool Update(float dt);
	void RenderDebug();
	void SetLuaState(lua_State* ls);

	bool guardPhysicsCheck();

	// Create
	bool addConstraintToWorld(btTypedConstraint* constraint, bool ignoreCollision = false);
	bool removeConstraintFromWorld(btTypedConstraint* constraint);
	IRigidBody* createRigidBody(const Object3D& m, const Mesh& c);
	btHingeConstraint* createHingeConstraint(btRigidBody* a, btRigidBody* b, const Vec3& pivA, const Vec3& pivB, const Vec3& axisA, const Vec3& axisB);
	btConeTwistConstraint* createConeTwistConstraint(btRigidBody* a, btRigidBody* b, btTransform ta, btTransform tb);

	// Clean-up
	void removeRigidBody(IRigidBody* rb, irr::scene::IAnimatedMesh* col);
	int getMeshUseCount(irr::scene::IAnimatedMesh* mesh);

	// Configuration
	Vec3 getGravity();
	void setGravity(const Vec3& grav);
	bool isPaused();
	void setPaused(bool v);
	float getStepFactor() { return stepFactor; }
	void setStepFactor(float v) { stepFactor = v; } // Multiplier to dt
	void setDebugMode(int v);
	void setIgnoreSameID(bool v) { collisionsIgnoreSameID = v; }

	// Helpers

	// Push to collision detection
	void appendToCollisionDetection(btCollisionObject* col, const PhysicsObject& phys);
	void removeFromCollisionDetection(btCollisionObject* col);

	void setFixedStep(float fs);
	void setMaxSubSteps(int sub);
private:
	irrBulletWorld* world = nullptr;

	float stepFactor = 1.0f;
	bool collisionsIgnoreSameID = false;

	// Callbacks
	void handleCollisions();
	void processCollisions();
	std::unordered_set<std::pair<btCollisionObject*, btCollisionObject*>, PairHash> lastCollisions;
	std::unordered_set<std::pair<btCollisionObject*, btCollisionObject*>, PairHash> currentCollisions;
	std::unordered_map<std::pair<btCollisionObject*, btCollisionObject*>, ContactInfo, PairHash> curData;
	std::unordered_map<btCollisionObject*, ColliderInfo> colliderPair;
	std::unordered_set<irr::scene::IAnimatedMesh*> meshesInUse;

	float fixedStep = 1.0f / 30.0f;
	int maxSubSteps = 8;
};