#pragma once
#include "Interfaces/SceneObject.h"
#include "Interfaces/Object3D.h"

class Application;
class Texture;
class Material;
class Vec2;

namespace irr {
    namespace scene {
        class ISceneNode;
        class IBillboardSceneNode;
    }
}

class Billboard : public SceneObject, public Object3D {
public:
    Billboard();
    Billboard(const Material& mat);
    void destroy() override;

    void loadMaterial(const Material& mat);

    Vec2 getSize();
    void setSize(const Vec2& s);

    irr::scene::ISceneNode* getNode() const override;
private:
    irr::scene::IBillboardSceneNode* bb = nullptr;
};

namespace Object::BillboardBind {
    void bind(Application* app);
}