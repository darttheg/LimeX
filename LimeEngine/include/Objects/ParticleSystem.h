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
    void applyPreset(int type); // Configures pfx to each type for you

    bool getLocal() const;
    void setLocal(bool v);

    void clear();
    void spark(int amount = 1);

    bool getActive() const;
    void setActive(bool v);

    bool hasOutgoingParticles() const; // Are there active particles in the scene from this emitter?

    void addAttractionAffector(const Vec3& pos, float spd, bool attract, const Vec3& affectAxis);
    void addFadeOutAffector(const Vec4& color, int timeMs);
    void addGravityAffector(const Vec3& grav, int timeToTakeOverMs);
    void addRotationAffector(const Vec3& rotSpd, const Vec3& pos);
    void addScalarAffector(float scalar);
    void clearAffectors();

    void loadMaterial(const Material& mat);

    // Affector-specific
    // All
    Vec2 getRates() const;
    void setRates(const Vec2&);

    float getSpread() const;
    void setSpread(float v);

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
    float getThickness() const;
    void setThickness(float v);

    // Cylinder
    float getHeight() const;
    void setHeight(float v);

    void destroy() override;
    void setDebug(bool v) override;

    irr::scene::ISceneNode* getNode() const override;
private:
    irr::scene::IParticleSystemSceneNode* pfx = nullptr;

    bool global = false;
};

namespace Object::ParticleSystemBind {
    void bind(lua_State* ls, RenderHelper* renh);
}