#pragma once
#include "Interfaces/SceneObject.h"
#include "Interfaces/Object3D.h"

extern "C" { struct lua_State; }
class RenderHelper;

class Vec2;
class Vec3;
class Material;

namespace irr {
    namespace scene {
        class ISceneNode;
        class IParticleSystemSceneNode;
    }
}

class ParticleSystem : public SceneObject, public Object3D {
public:
    ParticleSystem();
    ParticleSystem(int type);

    int getType() const;
    void setType(int type); // Set the emitter type

    bool getLocal() const;
    void setLocal(bool v);

    void clear();
    void spark(int amount = 1);

    bool getActive() const;
    void setActive(bool v);

    void addAttractionAffector(const Vec3& pos, float spd, bool attract, const Vec3& affectAxis);
    void addFadeOutAffector(const Vec4& color, int timeMs);
    void addGravityAffector(const Vec3& grav, int timeToTakeOverMs);
    void addRotationAffector(const Vec3& rotSpd, const Vec3& pos);
    void addScalarAffector(float scalar);
    void clearAffectors();

    void loadMaterial(const Material& mat);

    void loadPointEmitter(const Vec3& dir);
    void loadBoxEmitter(const Vec3& dir, const Vec3& scale);
    void loadSphereEmitter(const Vec3& dir, const Vec3& center, float radius);
    void loadRingEmitter(const Vec3& dir, const Vec3& center, float radius, float thickness);
    void loadCylinderEmitter(const Vec3& dir, const Vec3& center, float radius, float len, bool outlineOnly = false);
    void loadUserParams();

    // Affector-specific
    // All
    Vec2 getRates() const;
    void setRates(const Vec2&);

    int getMaxAngle() const;
    void setMaxAngle(int v);

    Vec2 getMinMaxLife() const;
    void setMinMaxLife(const Vec2& v);

    Vec2 getMinMaxScale() const;
    void setMinMaxScale(const Vec2& scale);

    Vec4 getMinColor() const;
    void setMinColor(const Vec4& v);

    Vec4 getMaxColor() const;
    void setMaxColor(const Vec4& v);

    // Box
    Vec3 getBoxSize() const;
    void setBoxSize(const Vec3& size);

    // Sphere, Ring, Cylinder
    float getRadius() const;
    void setRadius(float v);

    // Ring
    float getRingThickness() const;
    void setRingThickness(float v);

    // Cylinder
    float getCylinderLength() const;
    void setCylinderLength(float v);

    void destroy() override;
    void setDebug(bool v) override;

    irr::scene::ISceneNode* getNode() const override;
private:
    irr::scene::IParticleSystemSceneNode* pfx = nullptr;

    int curType = 0; // Point, Box, Sphere, Ring, Cylinder
    bool global = false;

    struct Vec4S { int r, g, b, a; };

    int minRate = 10;
    int maxRate = 25;
    int minLife = 1000;
    int maxLife = 5000;
    int maxAng = 0; // Omnidirectional
    float minScale = 1.0f;
    float maxScale = 1.0f;
    Vec4S minColor{ 255, 255, 255, 255 };
    Vec4S maxColor{ 255, 255, 255, 255 };
};

namespace Object::ParticleSystemBind {
    void bind(lua_State* ls, RenderHelper* renh);
}