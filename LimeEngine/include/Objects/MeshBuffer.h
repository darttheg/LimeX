#pragma once

extern "C" { struct lua_State; }

class Vec2;
class Vec3;
class Vec4;
class Renderer;
class Mesh;

#include "sol/forward.hpp"

namespace irr {
    namespace scene {
        class ISceneNode;
        class IAnimatedMeshSceneNode;
        class IShadowVolumeSceneNode;
        class IOctreeSceneNode;
        class IMesh;
		class IMeshBuffer;
    }

	namespace core {
		template<class T> class aabbox3d;
	}
}

class MeshBuffer {
public:
	MeshBuffer();
	MeshBuffer(const Mesh& m);

	bool loadMesh(const Mesh& m, int layer = 0);

	bool pushFace(const Vec3& v1, const Vec3& v2, const Vec3& v3,
		const Vec3& n1, const Vec3& n2, const Vec3& n3,
		const Vec2& uvw1, const Vec2& uvw2, const Vec2& uvw3,
		const Vec4& c1, const Vec4& c2, const Vec4& c3);
	bool pushQuad(const Vec3& p1, const Vec3& p2, const Vec3& p3, const Vec3& p4,
		const Vec3& n, const Vec2& uv1, const Vec2& uv2, const Vec2& uv3, const Vec2& uv4,
		const Vec4& c);
	void clear();
	sol::object purge();

	void recalculateBoundingBox();
	int getVertexCount() const;
	int getRefCount() const;

	irr::scene::IMeshBuffer* getBuffer() const { return buffer; }
private:
	irr::scene::IMeshBuffer* buffer;
	int currentIndex = 0;
};

namespace Object::MeshBufferBind {
    void bind(lua_State* ls, Renderer* rend);
}