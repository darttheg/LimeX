#include "Objects/ShadowVolume.h"
#include "Objects/IrrShadowVolume.h"

#include <sol/sol.hpp>

#include "Objects/Mesh.h"
#include "Renderer.h"

Renderer* r = nullptr;

ShadowVolume::ShadowVolume(const Mesh& src) {
	if (!r->guardRenderingCheck() || !r->isStencilBufferActive()) return;

	shadow = new ShadowVolumeSceneNode(src.getMesh(), src.getNode()->getSceneManager()->getRootSceneNode(), src.getNode()->getSceneManager(), 0);
	if (shadow) {
		shadow->setPosition(src.getNode()->getPosition());
		shadow->setRotation(src.getNode()->getRotation());
		shadow->setScale(src.getNode()->getScale());
	}
}

void ShadowVolume::destroy() {
	if (shadow) shadow->remove();
	shadow = nullptr;
}

void ShadowVolume::setDebug(bool v) {
	if (v) {
	}
	else {
	}
}

bool ShadowVolume::loadMesh(const Mesh& src) {
	if (!shadow || !r->isStencilBufferActive()) return false;
	return shadow->loadMesh(src.getMesh());
}

irr::scene::ISceneNode* ShadowVolume::getNode() const {
	return shadow;
}

void Object::ShadowVolumeBind::bind(lua_State* ls, Renderer* re) {
	r = re;

	sol::state_view view(ls);
	sol::usertype<ShadowVolume> obj = view.new_usertype<ShadowVolume>(
		"ShadowVolume",
		sol::constructors<ShadowVolume(const Mesh&)>(),

		sol::base_classes, sol::bases<Object3D>(),
		sol::meta_function::type, [](const ShadowVolume&) { return "ShadowVolume"; }
	);

	obj[sol::meta_function::to_string] = [](const ShadowVolume& v) {
		return "ShadowVolume";
		};

	// Object ShadowVolume, An object that draws shadows using a `Mesh` as its shape. **This object is not functional if the stencil buffer is not enabled**.
	// Inherits Object3D

	// Constructor Mesh mesh

	// Loads a new `Mesh` for this `ShadowVolume` to be shaped after.
	// Returns boolean
	obj.set_function("load", &ShadowVolume::loadMesh);

	// End Object
}