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

    void setType(int type);
    Vec4 getDiffuseColor() const;
    void setDiffuseColor(const Vec4& c);
    Vec4 getAmbientColor() const;
    void setAmbientColor(const Vec4& c);
    Vec4 getSpecularColor() const;
    void setSpecularColor(const Vec4& c);

    // Problem is, getting and setting these edits the colors too.
    // Maybe store the colors and then apply intensities etc? This will probably just be for diffuse though.
    float getIntensity() const;
    void setIntensity(float i);

    Vec3 getAttenuation() const;
    void setAttenuation(const Vec3& att);
    float getRadius() const; // Point
    void setRadius(float r); // Point
    float getFallOff() const; // Spotlight
    void setFallOff(float f); // Spotlight
    Vec2 getCones() const; // Spotlight
    void setCones(const Vec2& c); // Spotlight

    irr::scene::ISceneNode* getNode() const override;
private:
    irr::scene::ILightSceneNode* light = nullptr;
};

namespace Object::LightBind {
    void bind(Application* app);
}