#include "Objects/ShadowVolume.h"
#include "Objects/IrrShadowVolume.h"

#include <sol/sol.hpp>

#include "Objects/Mesh.h"
#include "RenderHelper.h"

RenderHelper* rh = nullptr;

ShadowVolume::ShadowVolume(const Mesh& src) {
	if (rh->guardRenderingCheck()) return;

	shadow = new ShadowVolumeSceneNode(src.getMesh(), src.getNode()->getSceneManager()->getRootSceneNode(), src.getNode()->getSceneManager(), 0);
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
	if (!shadow) return false;
	return shadow->loadMesh(src.getMesh());
}

irr::scene::ISceneNode* ShadowVolume::getNode() const {
	return shadow;
}

void Object::ShadowVolumeBind::bind(lua_State* ls, RenderHelper* renh) {
	rh = renh;

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

	// DEPRECATED ShadowVolume, An object that draws shadows using a `Mesh` as its shape.
	// Inherits Object3D

	// Constructor Mesh mesh

	// Loads a new `Mesh` for this `ShadowVolume` to be shaped after.
	// Returns boolean
	obj.set_function("load", &ShadowVolume::loadMesh);

	// End Object
}