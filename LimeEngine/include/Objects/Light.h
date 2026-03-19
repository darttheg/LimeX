#pragma once
#include "Interfaces/SceneObject.h"
#include "Interfaces/Object3D.h"

class Application;
class Vec2;
class Vec3;
class Vec4;

namespace irr {
    namespace scene {
        class ISceneNode;
        class ILightSceneNode;
    }
}

class Light : public SceneObject, public Object3D {
public:
    Light();
    Light(int type);
    void destroy() override;
    void setDebug(bool v) override;

    int getType() const;
    void setType(int type);
    Vec4 getDiffuseColor() const;
    void setDiffuseColor(const Vec4& c);
    Vec4 getAmbientColor() const;
    void setAmbientColor(const Vec4& c);
    Vec4 getSpecularColor() const;
    void setSpecularColor(const Vec4& c);

    float getIntensity() const;
    void setIntensity(float i);

    Vec3 getAttenuation() const;
    void setAttenuation(const Vec3& att);
    float getRadius() const; // Point
    void setRadius(float r); // Point
    float getFalloff() const; // Spotlight
    void setFalloff(float f); // Spotlight
    Vec2 getCones() const; // Spotlight
    void setCones(const Vec2& c); // Spotlight
    
    bool getShadows() const;
    void setShadows(bool v);

    irr::scene::ISceneNode* getNode() const override;
private:
    irr::scene::ILightSceneNode* light = nullptr;
    struct Vec4S { float x, y, z, w; };
    Vec4S diffuse;
};

namespace Object::LightBind {
    void bind(Application* app);
}