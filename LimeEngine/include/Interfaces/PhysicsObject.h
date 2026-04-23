#pragma once

#include <memory>

#include <sol/sol.hpp>
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
    PhysicsObject();
    virtual btCollisionObject* getCollisionObject() const = 0;
    virtual irr::scene::ISceneNode* getNode() const = 0;
    virtual void destroy() = 0;
    sol::object i_destroy();

    void createCallbacks();
    void insertIntoCallbacks();
    void removeFromCallbacks();

    std::shared_ptr<Event> onEnter = nullptr;
    std::shared_ptr<Event> onInside = nullptr;
    std::shared_ptr<Event> onExit = nullptr;

    std::shared_ptr<Event> getEnterEvent() { return onEnter; }
    std::shared_ptr<Event> getInsideEvent() { return onInside; }
    std::shared_ptr<Event> getExitEvent() { return onExit; }
};

namespace Interface::PhysicsObjectBind {
    void bind(lua_State* ls, PhysicsManager* p);
}