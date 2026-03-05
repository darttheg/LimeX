#include "Objects/Texture.h"
#include "Application.h"
#include "DebugConsole.h"
#include "Renderer.h"
#include "RenderHelper.h"

#include "irrlicht.h"

#include "Objects/Vec2.h"
#include "Objects/Vec4.h"

static Application* a;
static Renderer* r;
static RenderHelper* rh;
static DebugConsole* d;

Texture::Texture() {
	texture = rh->createTexture(1, 1, "blank");
}

Texture::Texture(const std::string& path) {
	texture = rh->createTexture(path);
}

Texture::Texture(irr::video::ITexture* tex) {
	texture = tex;
}

Texture::Texture(int w, int h, const std::string& name) {
	if (!rh->guardRenderingCheck()) return;

	texture = rh->createTexture(w, h, name);
}

sol::object Texture::remove() {
	if (!r->removeTexture(texture))
		d->Warn("Could not remove texture: it is not valid!");
	texture = nullptr;
	return sol::make_object(a->GetLuaState(), sol::nil);
}

Vec2 Texture::getSize() {
	if (!texture) return Vec2();
	return Vec2((float)texture->getSize().Width, (float)texture->getSize().Height);
}

void Texture::write(const std::string& outPath) {
	d->Warn("INCOMPLETE FUNCTION! Needs write texture thread.");
}

void Texture::crop(const Vec2& tl, const Vec2& br) {
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

	texture = rh->cropTexture(texture, pos, dim);
	if (!texture) {
		d->Warn("Crop failed: Failed to crop texture!");
		return;
	}
}

void Texture::append(const Texture& other, const Vec2& pos) {
	if (!texture) {
		d->Warn("Append failed: There is no valid Texture!");
		return;
	}

	irr::video::ITexture* t = rh->appendTexture(texture, other.texture, pos);
	if (!t) {
		d->Warn("Append failed: Could not append Texture!");
		return;
	}

	texture = t;
}

Vec4 Texture::getColor(const Vec2& pos) {
	if (!texture) {
		d->Warn("GetColor failed: There is no valid Texture!");
		return Vec4();
	}

	return rh->getColor(texture, pos);
}

void Texture::setColor(const Vec2& pos, const Vec4& color) {
	if (!texture) {
		d->Warn("SetColor failed: There is no valid Texture!");
		return;
	}

	rh->setColor(texture, pos, color);
}

void Texture::key(const Vec4& color) {
	if (!texture) {
		d->Warn("KeyColor failed: There is no valid Texture!");
		return;
	}

	rh->keyColor(texture, color);
}

std::string Texture::getPath() const {
	if (!texture) {
		d->Warn("GetPath failed: There is no valid Texture!");
		return "";
	}

	return texture->getName().getPath().c_str();
}

int Texture::getRefCount() {
	return texture ? texture->getReferenceCount() : -1;
}

void Object::TextureBind::bind(Application* app) {
	a = app;
	r = a->GetRenderer();
	rh = a->GetRenderHelper();
	d = a->GetDebugConsole();

	sol::state_view view(a->GetLuaState());
	sol::usertype<Texture> obj = view.new_usertype<Texture>(
		"Texture",
		sol::constructors<Texture(), Texture(const std::string&), Texture(int, int, const std::string&)>(),
		sol::meta_function::type, [](const Texture&) { return "Texture"; }
	);

	obj[sol::meta_function::to_string] = [](const Texture& v) {
		return v.getPath();
		};

	// Object Texture, A texture that is the foundation for all images for 2D and 3D objects.

	// Constructor
	// Constructor number w, number h, string? name
	// Constructor string path

	// Returns the path of this `Texture`.
	// Returns string
	obj.set_function("getPath", &Texture::getPath);

	// Returns the dimensions of this `Texture`.
	// Returns Vec2
	obj.set_function("getSize", &Texture::getSize);

	// Writes the `Texture` to path `path`.
	// Params string path
	// Returns void
	obj.set_function("write", &Texture::write);

	// Crops the `Texture` to the dimensions provided. Be mindful as cropping creates a new `Texture` in the renderer.
	// Params Vec2 topLeft, Vec2 bottomRight
	// Returns void
	obj.set_function("crop", &Texture::crop);

	// Appends another `Texture` onto this `Texture`.
	// Params Texture toAppend, Vec2 pos
	// Returns void
	obj.set_function("append", &Texture::append);

	// Returns the color of the pixel at `pos` in this `Texture`.
	// Params Vec2 pos
	// Returns Vec4
	obj.set_function("getColor", &Texture::getColor);

	// Replaces the pixel at `pos` with a pixel of color `color`.
	// Params Vec2 pos, Vec4 color
	// Returns void
	obj.set_function("setColor", &Texture::setColor);

	// Removes the color `keyColor` from anywhere in this `Texture`.
	// Params Vec4 keyColor
	// Returns void
	obj.set_function("keyColor", &Texture::key);

	// Returns the reference count for this `Texture`.
	// Returns number
	obj.set_function("getReferenceCount", &Texture::getRefCount);

	// Purges this `Texture`, effectively removing it from memory. Objects using this `Texture` will use an engine-defined `Texture` instead, but it is recommended to remove references to this `Texture` first.
	// Returns nil
	obj.set_function("purge", &Texture::remove);

	// End Object
}