#pragma once
#include "Interfaces/SceneObject.h"
#include "Interfaces/Object3D.h"

class Application;
class Texture;
class Material;
class Vec2;
class Vec3;
class Vec4;
class MeshBuffer;

namespace irr {
    namespace scene {
        class ISceneNode;
        class IAnimatedMeshSceneNode;
        class IShadowVolumeSceneNode;
        class IOctreeSceneNode;
        class IMesh;
    }
}

class Mesh : public SceneObject, public Object3D {
public:
    // Loading mesh sets hardware mapping hint to static on loading from path
    Mesh(); 
    Mesh(const std::string& path); // Load via path
    Mesh(const MeshBuffer& mb); // Load via MeshBuffer
    Mesh(irr::scene::IMesh* m);
    void destroy() override;
    void setDebug(bool v) override;

    irr::scene::ISceneNode* getNode() const override;

    bool loadMesh(const std::string& path);
    bool loadMeshBuffer(const MeshBuffer& mb);
    bool loadMaterial(int layer, const Material& mat);
    bool loadMaterial(const Material& mat);
    int getMaterialCount() const;

    void setHardwareHint(int hint);
    int getVertexCount() const;

    void setColor(const Vec4& col);
    Vec4 getColor() const;

    bool getSimpleCollision() const;
    void setSimpleCollision(bool v);

    /*
    bool getIsOctree() const;
    void setBecomeOctree(bool v);
    */

    bool getShadows() const;
    void setShadows(bool v);
    void updateShadowCasting();

    void clear();
    void purge();
private:
    bool col = false;
    irr::scene::ISceneNode* src = nullptr;
    irr::scene::IShadowVolumeSceneNode* sh = nullptr;

    irr::scene::IMesh* getMesh() const;
};

namespace Object::MeshBind {
    void bind(Application* app);
}