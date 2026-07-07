#pragma once
#include "Interfaces/SceneObject.h"
#include "Interfaces/Object3D.h"

extern "C" { struct lua_State; }

class ShadowVolumeSceneNode;
class Renderer;
class Mesh;

namespace irr {
    namespace scene {
        class ISceneNode;
    }
}

class ShadowVolume : public SceneObject, public Object3D {
public:
    ShadowVolume(const Mesh& src);
    void destroy() override;
    void setDebug(bool v) override;

    bool loadMesh(const Mesh& src);

    irr::scene::ISceneNode* getNode() const override;
private:
    ShadowVolumeSceneNode* shadow = nullptr;
};

namespace Object::ShadowVolumeBind {
    void bind(lua_State* ls, Renderer* re);
}