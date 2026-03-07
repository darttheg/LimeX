#include "Objects/MeshBuffer.h"

#include "Application.h"
#include "DebugConsole.h"
#include "Renderer.h"
#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"

#include "irrlicht.h"

static Application* a;
static DebugConsole* d;
static Renderer* r;

MeshBuffer::MeshBuffer() {
	if (r->guardRenderingCheck()) {
		buffer = new irr::scene::SMeshBuffer();
	}
}

void MeshBuffer::pushFace(const Vec3& v1, const Vec3& v2, const Vec3& v3, const Vec3& n1, const Vec3& n2, const Vec3& n3, const Vec2& uvw1, const Vec2& uvw2, const Vec2& uvw3, const Vec4& c1, const Vec4& c2, const Vec4& c3) {
	if (!buffer) return;
	irr::scene::SMeshBuffer* b = static_cast<irr::scene::SMeshBuffer*>(buffer);
	if (!b) return;

    b->Vertices.reallocate(b->Vertices.size() + 3);
    b->Vertices.set_used(b->Vertices.size() + 3);

    b->Vertices[currentIndex] = irr::video::S3DVertex(irr::core::vector3df(v1.getX(), v1.getY(), v1.getZ()), irr::core::vector3df(n1.getX(), n1.getY(), n1.getZ()), irr::video::SColor(c1.getW(), c1.getX(), c1.getY(), c1.getZ()), irr::core::vector2df(uvw1.getX(), uvw1.getY()));
    b->Vertices[currentIndex + 1] = irr::video::S3DVertex(irr::core::vector3df(v2.getX(), v2.getY(), v2.getZ()), irr::core::vector3df(n2.getX(), n2.getY(), n2.getZ()), irr::video::SColor(c2.getW(), c2.getX(), c2.getY(), c2.getZ()), irr::core::vector2df(uvw2.getX(), uvw2.getY()));
    b->Vertices[currentIndex + 2] = irr::video::S3DVertex(irr::core::vector3df(v3.getX(), v3.getY(), v3.getZ()), irr::core::vector3df(n3.getX(), n3.getY(), n3.getZ()), irr::video::SColor(c3.getW(), c3.getX(), c3.getY(), c3.getZ()), irr::core::vector2df(uvw3.getX(), uvw3.getY()));

    b->Indices.reallocate(b->Indices.size() + 3);
    b->Indices.set_used(b->Indices.size() + 3);

    b->Indices[currentIndex] = currentIndex;
    b->Indices[currentIndex + 1] = currentIndex + 1;
    b->Indices[currentIndex + 2] = currentIndex + 2;

    b->recalculateBoundingBox();

    currentIndex += 3;
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
	if (!buffer) return;
	irr::scene::SMeshBuffer* b = static_cast<irr::scene::SMeshBuffer*>(buffer);
	if (!b) return;

	b->Vertices.clear();
	b->Indices.clear();
	currentIndex = 0;
	b->BoundingBox.reset(0, 0, 0);
}

sol::object MeshBuffer::purge() {
	if (!r->removeBuffer(buffer))
		d->Warn("Could not purge buffer: it is not valid!");
	buffer = nullptr;
	return sol::make_object(a->GetLuaState(), sol::nil);
}

int MeshBuffer::getRefCount() const {
	return buffer ? buffer->getReferenceCount() : 0;
}

void Object::MeshBufferBind::bind(Application* app) {
	a = app;
	d = app->GetDebugConsole();
	r = app->GetRenderer();

	// Object MeshBuffer, A container for vertices.

	// Constructor

	sol::state_view view(a->GetLuaState());
	sol::usertype<MeshBuffer> obj = view.new_usertype<MeshBuffer>(
		"MeshBuffer",
		sol::constructors<MeshBuffer()>(),

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

	// Pushes a face to this `MeshBuffer`.
	// Params Vec3 pos1, Vec3 pos2, Vec3 pos3, Vec3 normal1, Vec3 normal2, Vec3 normal3, Vec3 uvw1, Vec3 uvw2, Vec3 uvw3, Vec4 color1, Vec4 color2, Vec4 color3
	// Returns void
	obj.set_function("pushFace", &MeshBuffer::pushFace);

	// Returns the reference count for this `MeshBuffer`.
	// Returns number
	obj.set_function("getReferenceCount", &MeshBuffer::getRefCount);

	// Clears this `MeshBuffer`. This will not remove itself from memory.
	// Returns void
	obj.set_function("clear", &MeshBuffer::clear);

	// Purges this `MeshBuffer`, effectively removing it from memory. Objects using this `MeshBuffer` will use an engine-defined `MeshBuffer` instead, but it is recommended to remove references to this `MeshBuffer` first.
	// Returns nil
	obj.set_function("purge", &MeshBuffer::purge);

	// End Object
}