#pragma once
#include "Interfaces/SceneObject.h"

extern "C" { struct lua_State; }

class RigidBody : public SceneObject {
public:
    // RigidBody();
private:
};

namespace Object::RigidBodyBind {
    // void bind(lua_State* ls);
}