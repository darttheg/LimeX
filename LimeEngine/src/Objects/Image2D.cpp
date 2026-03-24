#include "Objects/Image2D.h"
#include "irrlicht.h"
#include <sol/sol.hpp>

#include "External/CGUIColoredText.h"
#include "RenderHelper.h"
#include "Objects/Texture.h"
#include "Objects/Vec2.h"

static RenderHelper* rh = nullptr;

Image2D::Image2D() {
	img = rh->createGUIImage();
	if (!img) return;

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

void Image2D::destroy() {
	if (img) img->remove();
	img = nullptr;
}

bool Image2D::loadTexture(const Texture& tx) {
	if (!img || !tx.getTexture()) return false;

	img->setImage(tx.getTexture());

	return true;
}

bool Image2D::getScaledFit() const {
	return img ? img->isImageScaled() : false;
}

void Image2D::setScaledFit(bool v) {
	if (img)
		img->setScaleImage(v);
}

irr::gui::IGUIElement* Image2D::getNode() const {
	return img;
}

void Object::Image2DBind::bind(lua_State* ls, RenderHelper* renh) {
	rh = renh;

	// Object Image2D, A basic 2D object to display images. Without loading a Texture, this object can be used as a container for other GUI objects if parented together.
	// Inherits Object2D

	// Constructor
	// Constructor Texture tex
	// Constructor Vec2 pos, Vec2 size

	sol::state_view view(ls);
	sol::usertype<Image2D> obj = view.new_usertype<Image2D>(
		"Image2D",
		sol::constructors<Image2D(), Image2D(const Texture& t), Image2D(const Vec2& pos, const Vec2& sz)>(),

		sol::base_classes, sol::bases<Object2D>(),
		sol::meta_function::type, [](const Image2D&) { return "Image2D"; },

		// Field boolean scaleToFit, Determines whether or not this `Image2D` scales to its size boundaries or not.
		"scaleToFit", sol::property(&Image2D::getScaledFit, &Image2D::setScaledFit)
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