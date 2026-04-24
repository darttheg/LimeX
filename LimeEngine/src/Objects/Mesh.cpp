#include "Objects/Mesh.h"

#include "Renderer.h"
#include "RenderHelper.h"
#include "DebugConsole.h"
#include "Objects/Texture.h"
#include "Objects/Material.h"
#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"
#include "Objects/MeshBuffer.h"

#include "irrlicht.h"

#include <sol/sol.hpp>

static Renderer* r;
static DebugConsole* d;
static RenderHelper* rh;

Mesh::Mesh() {

}

Mesh::Mesh(const std::string& path) : Mesh() {
	loadMesh(path);
}

Mesh::Mesh(const MeshBuffer& mb) : Mesh() {
	loadMeshBuffer(mb);
}

Mesh::Mesh(irr::scene::IMesh* m) {
	irr::scene::SAnimatedMesh* amesh = new irr::scene::SAnimatedMesh(m);
	src = rh->createAnimatedMesh(amesh);
}

void Mesh::destroy() {
	if (src) src->remove();
	src = nullptr;
	col = false;
}

void Mesh::setDebug(bool v) {
	if (v) {
		if (src) src->setDebugDataVisible(
			irr::scene::EDS_BBOX |
			// irr::scene::EDS_NORMALS |
			irr::scene::EDS_MESH_WIRE_OVERLAY);
		if (dVisual) { dVisual->drop(); dVisual->remove(); }
		dVisual = rh->createDebugNode(DEBUG3D_TYPE::MESH);
		dVisual->setParent(getNode());
	} else {
		if (src) src->setDebugDataVisible(irr::scene::EDS_OFF);
		if (dVisual) {
			dVisual->drop();
			dVisual->remove();
		}
	}
}

bool Mesh::loadMesh(const std::string& path) {
	if (src) {
		if (getMesh()->getReferenceCount() > 1) // Necessary? Probably nice to know though.
			d->Warn("This Mesh is loading a new Mesh, but the old Mesh is not held elsewhere! Purge Meshes if they are to be unused.");

		src->remove();
	}

	src = rh->createAnimatedMesh(rh->createMesh(path));
	if (!src || src->getMesh() == src->getSceneManager()->getMesh("meshes/error.obj")) return false;

	setDebug(Object3D::getDebug());
	src->setAnimationSpeed(0.0f);

	// Reapply collision state
	bool cur = col;
	col = !col;
	setSimpleCollision(cur);

	return src;
}

bool Mesh::loadMeshBuffer(const MeshBuffer& mb) {
	if (!mb.getBuffer()) return false;
	if (mb.getBuffer()->getVertexCount() == 0) return false;

	irr::scene::SMesh* mesh = new irr::scene::SMesh();
	mesh->addMeshBuffer(mb.getBuffer());
	mesh->recalculateBoundingBox();

	irr::scene::SAnimatedMesh* amesh = new irr::scene::SAnimatedMesh();
	amesh->addMesh(mesh);
	mesh->drop();

	if (!src) src = rh->createEmptyMesh();
	src->setMesh(amesh);
	amesh->drop();

	setDebug(Object3D::getDebug());

	// Reapply collision state
	bool cur = col;
	col = !col;
	setSimpleCollision(cur);

	return true;
}

bool Mesh::loadMaterial(const Material& mat, int layer) {
	if (!src) return false;

	if (layer < 0) layer = 0;
	if (layer >= src->getMaterialCount()) layer = src->getMaterialCount() - 1;

	src->getMaterial(layer) = mat.getMaterial();
	return true;
}

bool Mesh::loadMaterial(const Material& mat) {
	return loadMaterial(mat, 0);
}

int Mesh::getMaterialCount() const {
	return src ? src->getMaterialCount() : 0;
}

void Mesh::setHardwareHint(int hint) {
	if (!src) return;

	if (!getMesh()) return;
	getMesh()->setHardwareMappingHint((irr::scene::E_HARDWARE_MAPPING)hint);
}

int Mesh::getVertexCount() const {
	if (!src) return 0;
	if (!getMesh()) return 0;

	return getMesh()->getMeshBuffer(0)->getVertexCount();
}

void Mesh::setColor(const Vec4& col) {
	if (!src) return;
	rh->setVertexColor(src, col);
}

Vec4 Mesh::getColor() const {
	if (!src) return Vec4();

	auto* verts = static_cast<irr::video::S3DVertex*>(src->getMesh()->getMeshBuffer(0)->getVertices());
	irr::video::SColor out = verts[0].Color;
	return Vec4(out.getRed(), out.getGreen(), out.getBlue(), out.getAlpha());
}

bool Mesh::getSimpleCollision() const {
	return src ? col : false;
}

void Mesh::setSimpleCollision(bool v) {
	if (!src || col == v) return;
	col = v;

	if (col) {
		irr::scene::ITriangleSelector* tri = rh->createTriangleSelector(src);
		if (!tri) return;
		src->setTriangleSelector(tri);
		tri->drop();
	} else {
		src->setTriangleSelector(nullptr);
	}
}

bool Mesh::getShadows() const {
	return src && sh;
}

void Mesh::setShadows(bool v) {
	if (!src) return;
	bool has = sh != nullptr;
	if (has == v) return;

	if (v) {
		sh = src->addShadowVolumeSceneNode();
	} else if (sh) {
		sh->remove();
		sh = nullptr;
	}
}

void Mesh::updateShadowCasting() {
	if (!src || !sh) return;
	sh->updateShadowVolumes();
}

int Mesh::getFrame() const {
	return src ? src->getFrameNr() : 0;
}

int Mesh::getFrameCount() const {
	return src ? src->getEndFrame() - src->getStartFrame() : 0;
}

void Mesh::recalculateBoundingBox() const {
	if (!src || !src->getMesh()) return;

	for (u32 i = 0; i < src->getMesh()->getMeshBufferCount(); ++i)
		src->getMesh()->getMeshBuffer(i)->recalculateBoundingBox();
}

void Mesh::setFrame(int f) {
	if (!src) return;
	src->setCurrentFrame(f);
}

void Mesh::clear() {
	if (!src) return;
	if (!getMesh()) return;

	if (getMesh()->getReferenceCount() == 1) // Necessary? Probably nice to know though.
		d->Warn("This Mesh is being cleared, but the old Mesh is not held elsewhere! Purge Meshes if they are to be unused.");

	src->remove();
}

void Mesh::purge() {
	if (!src) return;
	irr::scene::IAnimatedMesh* out = src->getMesh();
	if (out) out->grab();
	src->remove();
	src = nullptr;
	if (out) {
		bool ok = r->removeMesh(out);
		if (ok) out->drop();
	}
}

bool Mesh::write(const std::string& outPath) {
	return r->writeMeshToPath(getMesh(), outPath);
}

irr::scene::IMesh* Mesh::getMesh() const {
	if (src) return src->getMesh();
	return nullptr;
}

irr::scene::ISceneNode* Mesh::getNode() const {
	return src;
}

void Object::MeshBind::bind(lua_State* ls, DebugConsole* dc, Renderer* rend, RenderHelper* renh) {
	r = rend;
	rh = renh;
	d = dc;

	// Object Mesh, A scene object capable of displaying a mesh.
	// Inherits Object3D

	sol::state_view view(ls);
	sol::usertype<Mesh> obj = view.new_usertype<Mesh>(
		"Mesh",
		sol::constructors<Mesh(), Mesh(const std::string&), Mesh(const MeshBuffer&)>(),

		sol::base_classes, sol::bases<Object3D>(),
		sol::meta_function::type, [](const Mesh&) { return "Mesh"; },

		// Field Vec4 vertexColor, Sets the color of all vertexes in this `Mesh`. The `Material` of this `Mesh` must have type `VertexAlpha` to take effect.
		"vertexColor", sol::property(
			[](Mesh& m) { return Vec4{ [&] { return m.getColor(); }, [&](auto v) { m.setColor(v); } }; },
			[](Mesh& m, const Vec4& v) { m.setColor(v); }
		),

		// Field boolean collision, Allows response to raypicks and other simple collision methods. (NOTE: This flag does not affect this `Mesh` when wrapped by a physics object.)
		"collision", sol::property(&Mesh::getSimpleCollision, &Mesh::setSimpleCollision),

		// Field number frame, Controls the current frame of animation.
		"frame", sol::property(&Mesh::getFrame, &Mesh::setFrame)
	);

	obj[sol::meta_function::to_string] = [](const Mesh& v) {
		return "Mesh";
		};

	// Constructor
	// Constructor string path
	// Constructor MeshBuffer buffer

	// Loads a 3D model into this `Mesh`.
	// Params string path
	// Params MeshBuffer buffer
	// Returns boolean
	obj.set_function("loadMesh",
		sol::overload(
			sol::resolve<bool(const std::string&)>(&Mesh::loadMesh),
			sol::resolve<bool(const MeshBuffer&)>(&Mesh::loadMeshBuffer)
		));

	// Loads a `Material` into this `Mesh`.
	// Params Material material, number layer
	// Params Material material
	// Returns boolean
	obj.set_function("loadMaterial",
		sol::overload(
			sol::resolve<bool(const Material&, int)>(&Mesh::loadMaterial),
			sol::resolve<bool(const Material&)>(&Mesh::loadMaterial)
		));

	// Returns the number of `Material` slots in this `Mesh`.
	// Returns number
	obj.set_function("getMaterialCount", &Mesh::getMaterialCount);

	// Returns the vertex count of this `Mesh`.
	// Returns number
	obj.set_function("getVertexCount", &Mesh::getVertexCount);

	// Returns the number of animation frames.
	// Returns number
	obj.set_function("getFrameCount", &Mesh::getFrameCount);

	// Recalculates the bounding box of this `Mesh`. This is useful for potential loading errors, where this `Mesh` stops rendering even if it is within the active `Camera`'s view.
	// Returns void
	obj.set_function("recalculateBoundingBox", &Mesh::recalculateBoundingBox);

	// Informs the graphics system of how this `Mesh` should be stored. By default, `Mesh` objects use Static. Use Dynamic (or more intensely, Stream) if the `Mesh` is updated frequently.
	// Params Lime.Enum.StorageHint hint
	// Returns void
	obj.set_function("setStorageHint", &Mesh::setHardwareHint);

	// Writes this `Mesh` to `path`.
	// Params string path
	// Returns boolean
	obj.set_function("write", &Mesh::write);

	// Clears the `MeshBuffer` from within this `Mesh`. This will not remove its `MeshBuffer` from memory.
	// Returns void
	obj.set_function("clear", &Mesh::clear);

	// Purges this `Mesh`, effectively removing its mesh buffer from memory. Objects using the `MeshBuffer` of this `Mesh` will use an engine-defined `Mesh` instead, but it is recommended to remove references to this `Mesh` first.
	// Returns nil
	obj.set_function("purge", &Mesh::purge);

	// End Object
}

// DEPRECATED

// Field boolean shadows, Enables shadows. If there is no light source, the scene will be dark until one is created.
// "shadows", sol::property(&Mesh::getShadows, &Mesh::setShadows),

// If shadows are enabled for this `Mesh`, this will update the projection of the shadow. Use this if light sources have moved.
// Returns void
// obj.set_function("updateShadow", &Mesh::updateShadowCasting);