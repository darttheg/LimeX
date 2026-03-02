#include "Objects/Billboard.h"

#include "Application.h"
#include "Renderer.h"
#include "DebugConsole.h"
#include "Objects/Texture.h"
#include "Objects/Material.h"

#include "Objects/Vec2.h"

#include "irrlicht.h"

static DebugConsole* d;
static Renderer* r;

Billboard::Billboard() {
	bb = r->createBillboardNode();
}

Billboard::Billboard(const Material& mat) {
	bb = r->createBillboardNode();
	loadMaterial(mat);
}

Vec2 Billboard::getSize() {
	if (!bb) return Vec2();

	return Vec2(bb->getSize().Width, bb->getSize().Height);
}

void Billboard::setSize(const Vec2& s) {
	if (!bb);

	bb->setSize(s.getY(), s.getX(), s.getX());
}

irr::scene::ISceneNode* Billboard::getNode() const { return bb; }

using namespace irr;
using namespace video;
void Billboard::loadMaterial(const Material& mat) {
	if (!bb) return;
	bb->getMaterial(0) = mat.getMaterial();
}

void Object::BillboardBind::bind(Application* a) {
	r = a->GetRenderer();
	d = a->GetDebugConsole();

	// Object Billboard, A plane that faces the active camera.
	// Inherits Object3D

	sol::state_view view(a->GetLuaState());
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