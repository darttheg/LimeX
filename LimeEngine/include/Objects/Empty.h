#pragma once
#include "Interfaces/SceneObject.h"
#include "Interfaces/Object3D.h"

extern "C" { struct lua_State; }
class Vec3;

class RenderHelper;

namespace irr {
    namespace scene {
        class ISceneNode;
    }
}

class Empty : public SceneObject, public Object3D {
public:
    Empty();
    Empty(const Vec3& pos);
    void destroy() override;
    void setDebug(bool v) override;

    irr::scene::ISceneNode* getNode() const override;
private:
    irr::scene::ISceneNode* emp = nullptr;
};

namespace Object::EmptyBind {
    void bind(lua_State* ls, RenderHelper* renh);
}