#pragma once
#include <string>
#include <unordered_map>
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
class RigidBody;
class Mesh;
class Object3D;

namespace irr {
	class IrrlichtDevice;
	namespace scene {
		class ISceneNode;
		class IAnimatedMesh;
	}
}

struct ContactInfo;

class PhysicsManager {
public:
	PhysicsManager(Renderer* owner, DebugConsole* debug);
	~PhysicsManager() = default;

	bool Init(irr::IrrlichtDevice* device);
	bool Update(float dt);
	void SetLuaState(lua_State* ls);

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

	// Helpers

	// Clean rigid body with src as its visual/collision shape.
	bool cleanRigidBodySource(irr::scene::IAnimatedMesh* src);
	void appendToMatchedRBSrc(irr::scene::IAnimatedMesh* src, RigidBody* rb);
	void appendToMatchedRBCol(irr::scene::IAnimatedMesh* col, RigidBody* rb);

	// Push to collision detection
	void appendToCollisionDetection(btCollisionObject* col, PhysicsObject* phys);
	void removeFromCollisionDetection(btCollisionObject* col);
private:
	irrBulletWorld* world = nullptr;

	float stepFactor = 1.0f;
	bool collisionsIgnoreSameID = false;

	std::unordered_map<irr::scene::IAnimatedMesh*, RigidBody*> rbMappedSrc;
	std::unordered_map<irr::scene::IAnimatedMesh*, RigidBody*> rbMappedCol;

	// Callbacks
	void handleCollisions();
	void processCollisions();
	std::set<std::pair<btCollisionObject*, btCollisionObject*>> lastCollisions;
	std::set<std::pair<btCollisionObject*, btCollisionObject*>> currentCollisions;
	std::map<std::pair<btCollisionObject*, btCollisionObject*>, ContactInfo> curData;
	std::unordered_map<btCollisionObject*, PhysicsObject*> colliderPair;
};