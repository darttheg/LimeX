#include "Objects/Image.h"
#include "Application.h"
#include "DebugConsole.h"
#include "Renderer.h"

#include "irrlicht.h"

#include "Objects/Vec2.h"
#include "Objects/Vec4.h"

static Renderer* r = nullptr;
static DebugConsole* d = nullptr;

Image::Image() {
	if (!r->guardRenderingCheck()) return;

	texture = r->createTexture(1, 1, "blank");
}

Image::Image(const std::string& path) {
	if (!r->guardRenderingCheck()) return;

	texture = r->createTexture(path);
}

Image::Image(int w, int h, const std::string& name) {
	if (!r->guardRenderingCheck()) return;

	texture = r->createTexture(w, h, name);
}

Vec2 Image::getSize() {
	if (!r->guardRenderingCheck() || !texture) return Vec2();
	return Vec2((float)texture->getSize().Width, (float)texture->getSize().Height);
}

void Image::write(const std::string& outPath) {
	d->Warn("INCOMPLETE FUNCTION! Needs write texture thread.");
}

void Image::crop(const Vec2& tl, const Vec2& br) {
	if (!r->guardRenderingCheck()) return;
	if (!texture) {
		d->Warn("Crop failed: There is no valid texture!");
		return;
	}

	if (tl.getX() < 0 || tl.getY() < 0 || br.getX() < 0 || br.getY() < 0) {
		d->Warn("Crop failed: Invalid dimensions!");
		return;
	}

	Vec2 pos = tl;
	Vec2 dim = br - tl;

	texture = r->cropTexture(texture, pos, dim);
	if (!texture) {
		d->Warn("Crop failed: Failed to crop image!");
		return;
	}
}

void Image::append(const Image& other, const Vec2& pos) {
	if (!r->guardRenderingCheck()) return;
	if (!texture) {
		d->Warn("Append failed: There is no valid Image!");
		return;
	}

	irr::video::ITexture* t = r->appendTexture(texture, other.texture, pos);
	if (!t) {
		d->Warn("Append failed: Could not append Image!");
		return;
	}

	texture = t;
}

Vec4 Image::getColor(const Vec2& pos) {
	if (!r->guardRenderingCheck()) return Vec4();
	if (!texture) {
		d->Warn("GetColor failed: There is no valid Image!");
		return Vec4();
	}

	return r->getColor(texture, pos);
}

void Image::setColor(const Vec2& pos, const Vec4& color) {
	if (!r->guardRenderingCheck()) return;
	if (!texture) {
		d->Warn("SetColor failed: There is no valid Image!");
		return;
	}

	r->setColor(texture, pos, color);
}

void Image::key(const Vec4& color) {
	if (!r->guardRenderingCheck()) return;
	if (!texture) {
		d->Warn("KeyColor failed: There is no valid Image!");
		return;
	}

	r->keyColor(texture, color);
}

std::string Image::getPath() const {
	if (!texture) {
		d->Warn("GetPath failed: There is no valid Image!");
		return "";
	}

	return texture->getName().getPath().c_str();
}

void Object::ImageBind::bind(Application* a) {
	r = a->GetRenderer();
	d = a->GetDebugConsole();

	sol::state_view view(a->GetLuaState());
	sol::usertype<Image> obj = view.new_usertype<Image>(
		"Image",
		sol::constructors<Image(), Image(const std::string&), Image(int, int, const std::string&)>()
	);

	obj[sol::meta_function::to_string] = [](const Image& v) {
		return v.getPath();
		};

	// Object Image

	// Constructor
	// Constructor number w, number h, string? name
	// Constructor string path

	// Returns the path of this Image.
	// Returns string
	obj.set_function("getPath", &Image::getPath);

	// Returns the dimensions of this Image.
	// Returns Vec2
	obj.set_function("getSize", &Image::getSize);

	// Writes the Image to a path.
	// Params string path
	// Returns void
	obj.set_function("write", &Image::write);

	// Crops the Image to the dimensions provided.
	// Params Vec2 topLeft, Vec2 bottomRight
	// Returns void
	obj.set_function("crop", &Image::crop);

	// Appends another Image onto this Image.
	// Params Image toAppend, Vec2 pos
	// Returns void
	obj.set_function("append", &Image::append);

	// Returns the color of the pixel at a position in this Image.
	// Params Vec2 pos
	// Returns Vec4
	obj.set_function("getColor", &Image::getColor);

	// Replaces the pixel at the provided position with the color provided.
	// Params Vec2 pos, Vec4 color
	// Returns void
	obj.set_function("setColor", &Image::setColor);

	// Removes the color keyColor from anywhere in the Image.
	// Params Vec4 keyColor
	// Returns void
	obj.set_function("keyColor", &Image::key);

	// End Object
}