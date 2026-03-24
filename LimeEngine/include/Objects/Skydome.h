#pragma once
#include "Interfaces/SceneObject.h"
#include "Interfaces/Object3D.h"

extern "C" { struct lua_State; }

class Texture;
class Material;
class RenderHelper;

namespace irr {
	namespace scene {
		class ISceneNode;
	}
}

class Skydome : public SceneObject, public Object3D {
public:
    Skydome();
    Skydome(const Material& mat);
    void destroy() override;
    void setDebug(bool v) override;

    void loadMaterial(const Material& mat);

    irr::scene::ISceneNode* getNode() const override { return sky; }
private:
    irr::scene::ISceneNode* sky = nullptr;
};

namespace Object::SkydomeBind {
    void bind(lua_State* ls, RenderHelper* renh);
}