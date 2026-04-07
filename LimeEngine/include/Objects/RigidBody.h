#pragma once
#include "Interfaces/SceneObject.h"
#include "Interfaces/PhysicsObject.h"
#include <sol/forward.hpp>

extern "C" { struct lua_State; }
class PhysicsManager;

class IRigidBody;
class btCollisionObject;
class Mesh;
class Vec3;
class Object3D;

namespace irr::scene {
    class ISceneNode;
}

class RigidBody : public SceneObject, public PhysicsObject {
public:
    btCollisionObject* getCollisionObject() const override;
    irr::scene::ISceneNode* getNode() const override;

    RigidBody(const Mesh& m);
    RigidBody(const Object3D& m, const Mesh& c);

    bool loadMesh(const Mesh& m);

    Vec3 getPosition();
    void setPosition(const Vec3& pos);

    Vec3 getRotation();
    void setRotation(const Vec3& rot);

    void setFriction(float f);
    float getFriction();
    void setAnisotropicFriction(const Vec3& f);
    Vec3 getAnisotropicFriction();

    void setMass(float f);
    float getMass();

    float getLinearDamping();
    void setLinearDamping(float f);
    float getAngularDamping();
    void setAngularDamping(float f);

    float getElasticity();
    void setElasticity(float f); // setRestitution

    // Apply forces

    Vec3 getCenterOfMass();
    // relativePosition is actually in world-space coordinates...
    void applyForce(const Vec3& force, const Vec3& relativePosition);
    void applyImpulse(const Vec3& force, const Vec3& relativePosition);
    void applyTorque(const Vec3& force, bool impulsive);
    void faceTarget(const Vec3& position);
    void clearForces();

    void applySimpleForce(float force, const Vec3& worldPos);
    void applySimpleImpulse(float force, const Vec3& worldPos);

    Vec3 getLinearVelocity();
    void setLinearVelocity(const Vec3& vel);
    Vec3 getAngularVelocity();
    void setAngularVelocity(const Vec3& vel);
    Vec3 getLinearFactor();
    void setLinearFactor(const Vec3& f);
    Vec3 getAngularFactor();
    void setAngularFactor(const Vec3& f);

    float getSleepingThreshold();
    void setSleepingThreshold(float f);
    bool getGhost();
    void setGhost(bool v);
    Vec3 getForward();

    void wakeUp(); // Wakes up sleeping object
    bool isSleeping();
    void setSleeping(bool v);
     
    sol::table getAttributes();

    void nullify() { src = nullptr; col = nullptr; }
    void destroy() override;

    void loadVisual(irr::scene::ISceneNode* visual);
private:
    IRigidBody* rb = nullptr;
    irr::scene::ISceneNode* src = nullptr;
    irr::scene::ISceneNode* col = nullptr;
    bool isGhost = false;
};

namespace Object::RigidBodyBind {
    void bind(lua_State* ls, PhysicsManager* ps);
}