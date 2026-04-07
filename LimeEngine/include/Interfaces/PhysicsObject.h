#pragma once

#include <memory>

#include <sol/forward.hpp>
extern "C" { struct lua_State; }

class PhysicsManager;
class Event;
class btCollisionObject;

namespace irr::scene {
    class ISceneNode;
}

struct ContactInfo;

// Interface for RigidBody3D and SoftBody3D
class PhysicsObject {
public:
    virtual ~PhysicsObject() = default;
    virtual btCollisionObject* getCollisionObject() const = 0;
    virtual irr::scene::ISceneNode* getNode() const = 0;

    sol::table getCollision();

    void createCallbacks();

    std::shared_ptr<Event> onEnter = nullptr;
    std::shared_ptr<Event> onInside = nullptr;
    std::shared_ptr<Event> onExit = nullptr;

    std::shared_ptr<Event> getEnterEvent() { return onEnter; }
    std::shared_ptr<Event> getInsideEvent() { return onInside; }
    std::shared_ptr<Event> getExitEvent() { return onExit; }

    sol::table curCollisionInfo;
    void setCollisionInfo(ContactInfo info, bool isB);
};

namespace Interface::PhysicsObjectBind {
    void bind(lua_State* ls, PhysicsManager* p);
}