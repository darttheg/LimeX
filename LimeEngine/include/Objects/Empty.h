#pragma once
#include "Interfaces/SceneObject.h"
#include "Interfaces/Object3D.h"

class Application;
class Texture;
class Material;
class Vec2;
class Vec3;

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

    irr::scene::ISceneNode* getNode() const override;
private:
    irr::scene::ISceneNode* emp = nullptr;
};

namespace Object::EmptyBind {
    void bind(Application* app);
}