#include "Objects/MeshBuffer.h"

#include "Renderer.h"
#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"
#include "Objects/Mesh.h"

#include "irrlicht.h"
#include <sol/sol.hpp>

static lua_State* l;
static Renderer* r;

MeshBuffer::MeshBuffer() {
	buffer = new irr::scene::SMeshBuffer();
}

MeshBuffer::MeshBuffer(const Mesh& m) {
	loadMesh(m, 0);
}

bool MeshBuffer::loadMesh(const Mesh& m, int layer) {
	auto* b = static_cast<irr::scene::IAnimatedMeshSceneNode*>(m.getNode());
	if (!b) return false;
	auto* out = b->getMesh()->getMeshBuffer(layer);
	if (out) buffer = out;
	return out;
}

static irr::scene::SMeshBuffer* editable(irr::scene::IMeshBuffer* b) {
	return dynamic_cast<irr::scene::SMeshBuffer*>(b);
}

static irr::video::S3DVertex makeVertex(const Vec3& p, const Vec3& n, const Vec2& uv, const Vec4& c) {
	return irr::video::S3DVertex(
		irr::core::vector3df(p.getX(), p.getY(), p.getZ()),
		irr::core::vector3df(n.getX(), n.getY(), n.getZ()),
		irr::video::SColor(c.getW(), c.getX(), c.getY(), c.getZ()),
		irr::core::vector2df(uv.getX(), uv.getY())
	);
}

static void addVertex(irr::scene::SMeshBuffer* b, const irr::video::S3DVertex& v) {
	if (b->Vertices.empty()) b->BoundingBox.reset(v.Pos);
	else b->BoundingBox.addInternalPoint(v.Pos);
	b->Vertices.push_back(v);
}

static bool addQuad(irr::scene::SMeshBuffer* b, const irr::video::S3DVertex v[4]) {
	const irr::u32 base = b->Vertices.size();
	if (base + 4 > 65536) return false;
	for (int i = 0; i < 4; i++) addVertex(b, v[i]);
	static const irr::u16 order[6] = { 0,1,2,0,2,3 };
	for (irr::u16 i : order) b->Indices.push_back((irr::u16)(base + i));
	b->setDirty();
	return true;
}

bool MeshBuffer::pushFace(const Vec3& v1, const Vec3& v2, const Vec3& v3, const Vec3& n1, const Vec3& n2, const Vec3& n3, const Vec2& uvw1, const Vec2& uvw2, const Vec2& uvw3, const Vec4& c1, const Vec4& c2, const Vec4& c3) {
	auto* b = editable(buffer);
	if (!b) return false;

	const irr::u32 base = b->Vertices.size();
	if (base + 3 > 65536) return false;

	addVertex(b, makeVertex(v1, n1, uvw1, c1));
	addVertex(b, makeVertex(v2, n2, uvw2, c2));
	addVertex(b, makeVertex(v3, n3, uvw3, c3));

	b->Indices.push_back((irr::u16)(base));
	b->Indices.push_back((irr::u16)(base + 1));
	b->Indices.push_back((irr::u16)(base + 2));

	b->setDirty();
	return true;
}

bool MeshBuffer::pushQuad(const Vec3& p1, const Vec3& p2, const Vec3& p3, const Vec3& p4, const Vec3& n, const Vec2& uv1, const Vec2& uv2, const Vec2& uv3, const Vec2& uv4, const Vec4& c) {
	auto* b = editable(buffer);
	if (!b) return false;

	const irr::video::S3DVertex v[4] = {
		makeVertex(p1, n, uv1, c),
		makeVertex(p2, n, uv2, c),
		makeVertex(p3, n, uv3, c),
		makeVertex(p4, n, uv4, c)
	};

	return addQuad(b, v);
}

void MeshBuffer::recalculateBoundingBox() {
	if (!buffer) return;
	buffer->recalculateBoundingBox();
}

int MeshBuffer::getVertexCount() const {
	if (!buffer) return 0;
	return buffer->getVertexCount();
}

void MeshBuffer::clear() {
	auto* b = editable(buffer);
	if (!b) return;

	b->Vertices.clear();
	b->Indices.clear();
	b->BoundingBox.reset(0, 0, 0);
	b->setDirty();
}

sol::object MeshBuffer::purge() {
	r->removeBuffer(buffer);
	buffer = nullptr;
	return sol::make_object(l, sol::nil);
}

int MeshBuffer::getRefCount() const {
	return buffer ? buffer->getReferenceCount() : 0;
}

void Object::MeshBufferBind::bind(lua_State* ls, Renderer* rend) {
	l = ls;
	r = rend;

	// Object MeshBuffer, A container for vertices.

	// Constructor
	// Constructor Mesh

	sol::state_view view(ls);
	sol::usertype<MeshBuffer> obj = view.new_usertype<MeshBuffer>(
		"MeshBuffer",
		sol::constructors<MeshBuffer(), MeshBuffer(const Mesh& m)>(),

		sol::meta_function::type, [](const MeshBuffer&) { return "MeshBuffer"; }
	);

	obj[sol::meta_function::to_string] = [](const MeshBuffer& v) {
		return "MeshBuffer";
		};

	// Recalculates the bounding box of this `MeshBuffer`. This should be called after modifying this `MeshBuffer`.
	// Returns void
	obj.set_function("recalculateBoundingBox", &MeshBuffer::recalculateBoundingBox);

	// Returns the vertex count of this `MeshBuffer`.
	// Returns number
	obj.set_function("getVertexCount", &MeshBuffer::getVertexCount);

	// Pushes a triangle to this `MeshBuffer`.
	// Params Vec3 pos1, Vec3 pos2, Vec3 pos3, Vec3 normal1, Vec3 normal2, Vec3 normal3, Vec3 uvw1, Vec3 uvw2, Vec3 uvw3, Vec4 color1, Vec4 color2, Vec4 color3
	// Returns boolean
	obj.set_function("pushTri", &MeshBuffer::pushFace);

	// Pushes a quad to this `MeshBuffer`.
	// Params Vec3 pos1, Vec3 pos2, Vec3 pos3, Vec3 pos4, Vec3 normal, Vec2 uv1, Vec2 uv2, Vec2 uv3, Vec2 uv4, Vec4 color
	// Returns boolean
	obj.set_function("pushQuad", &MeshBuffer::pushQuad);

	// Returns the reference count for this `MeshBuffer`.
	// Returns number
	obj.set_function("getReferenceCount", &MeshBuffer::getRefCount);

	// Clears this `MeshBuffer`. This will not remove itself from memory.
	// Returns void
	obj.set_function("clear", &MeshBuffer::clear);

	// Purges this `MeshBuffer`, effectively removing it from memory. Objects using this `MeshBuffer` will use an engine-defined `MeshBuffer` instead, but it is recommended to remove references to this `MeshBuffer` first.
	// Returns nil
	obj.set_function("purge", &MeshBuffer::purge);

	// Loads the `MeshBuffer` stored within the provided `Mesh`.
	// Params Mesh mesh
	// Params Mesh mesh, number? layer
	// Returns boolean
	obj.set_function("loadMesh", &MeshBuffer::loadMesh);

	// End Object
}