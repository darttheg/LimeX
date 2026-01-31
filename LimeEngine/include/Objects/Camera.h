#pragma once
#include "Interfaces/SceneObject.h"
#include "Interfaces/Object3D.h"

class Application;

class Camera : public SceneObject, public Object3D {
public:
    
private:
};

namespace Object::CameraBind {
    void bind(Application* app);
}