#include "Objects/Image2D.h"
#include "irrlicht.h"

#include "External/CGUIColoredText.h"
#include "Application.h"
#include "DebugConsole.h"
#include "Renderer.h"
#include "Objects/Texture.h"
#include "Objects/Vec2.h"

static DebugConsole* d = nullptr;
static Renderer* r = nullptr;

Image2D::Image2D() {
	img = r->createGUIImage();

	Object2D::createEvents();
}

Image2D::Image2D(const Texture& t) : Image2D() {
	if (!img) return;

	loadTexture(t);
}

Image2D::Image2D(const Vec2& pos, const Vec2& sz) : Image2D() {
	if (!img) return;

	Object2D::setPosition(pos);
	Object2D::setSize(sz);
}

bool Image2D::loadTexture(const Texture& tx) {
	if (!img || !tx.getTexture()) return false;

	img->setImage(tx.getTexture());

	return true;
}

irr::gui::IGUIElement* Image2D::getNode() const {
	return img;
}

void Object::Image2DBind::bind(Application* a) {
	r = a->GetRenderer();
	d = a->GetDebugConsole();

	// Object Image2D, A basic 2D object to display images. Without loading a Texture, this object can be used as a container for other GUI objects if parented together.
	// Inherits Object2D

	// Constructor
	// Constructor Texture tex
	// Constructor Vec2 pos, Vec2 size

	sol::state_view view(a->GetLuaState());
	sol::usertype<Image2D> obj = view.new_usertype<Image2D>(
		"Image2D",
		sol::constructors<Image2D(), Image2D(const Texture& t), Image2D(const Vec2& pos, const Vec2& sz)>(),

		sol::base_classes, sol::bases<Object2D>(),
		sol::meta_function::type, [](const Image2D&) { return "Image2D"; }
	);

	obj[sol::meta_function::to_string] = [](const Image2D& v) {
		return "Image2D";
		};

	// Loads `texture` into this object.
	// Params Texture texture
	// Returns boolean
	obj.set_function("loadTexture", &Image2D::loadTexture);

	// End Object
}