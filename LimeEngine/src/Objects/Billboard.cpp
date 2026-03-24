#include "Objects/Billboard.h"

#include "RenderHelper.h"
#include "Objects/Material.h"

#include "Objects/Vec2.h"

#include "irrlicht.h"
#include <sol/sol.hpp>

static RenderHelper* rh;

Billboard::Billboard() {
	bb = rh->createBillboardNode();
}

Billboard::Billboard(const Material& mat) {
	bb = rh->createBillboardNode();
	loadMaterial(mat);
}

void Billboard::destroy() {
	if (bb) bb->remove();
	bb = nullptr;
}

void Billboard::setDebug(bool v) {
}

Vec2 Billboard::getSize() {
	if (!bb) return Vec2();

	return Vec2(bb->getSize().Width, bb->getSize().Height);
}

void Billboard::setSize(const Vec2& s) {
	if (!bb) return;
	bb->setSize(s.getY(), s.getX(), s.getX());
}

irr::scene::ISceneNode* Billboard::getNode() const { return bb; }

using namespace irr;
using namespace video;
void Billboard::loadMaterial(const Material& mat) {
	if (!bb) return;
	bb->getMaterial(0) = mat.getMaterial();
}

void Object::BillboardBind::bind(lua_State* ls, RenderHelper* renh) {
	rh = renh;

	// Object Billboard, A plane that faces the active camera.
	// Inherits Object3D

	sol::state_view view(ls);
	sol::usertype<Billboard> obj = view.new_usertype<Billboard>(
		"Billboard",
		sol::constructors<Billboard(), Billboard(const Material & material)>(),

		sol::base_classes, sol::bases<Object3D>(),
		sol::meta_function::type, [](const Billboard&) { return "Billboard"; },

		// Field Vec2 size, The width and height of this Billboard.
		"size", sol::property(
			[](Billboard& c) { return Vec2{ [&] { return c.getSize(); }, [&](auto v) { c.setSize(v); } }; },
			[](Billboard& c, const Vec2& v) { c.setSize(v); }
		)
	);

	obj[sol::meta_function::to_string] = [](const Billboard& v) {
		return "Billboard";
		};

	// Constructor
	// Constructor Material material

	// Loads a `Material` into this `Billboard`.
	// Params Material material
	// Returns void
	obj.set_function("loadMaterial", &Billboard::loadMaterial);

	// End Object
}