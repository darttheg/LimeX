#include "Objects/Texture.h"

#include "Renderer.h"
#include "RenderHelper.h"

#include "irrlicht.h"

#include "Objects/Vec2.h"
#include "Objects/Vec4.h"

#include <sol/sol.hpp>

static lua_State* l;
static Renderer* r;
static RenderHelper* rh;

Texture::Texture() {
	// texture = rh->createTexture(1, 1, "blank");
	texture = nullptr;
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

sol::object Texture::purge() {
	r->removeTexture(texture);
	texture = nullptr;
	return sol::make_object(l, sol::nil);
}

Vec2 Texture::getSize() {
	if (!texture) return Vec2();
	return Vec2((float)texture->getSize().Width, (float)texture->getSize().Height);
}

bool Texture::write(const std::string& outPath) {
	return false;
	// d->Warn("INCOMPLETE FUNCTION! Needs write texture thread.");
}

bool Texture::crop(const Vec2& tl, const Vec2& br) {
	if (!texture) return false;

	if (tl.getX() < 0 || tl.getY() < 0 || br.getX() < 0 || br.getY() < 0) return false;

	Vec2 pos = tl;
	Vec2 dim = br - tl;

	texture = rh->cropTexture(texture, pos, dim);
	return texture;
}

bool Texture::append(const Texture& other, const Vec2& pos) {
	if (!texture) return false;

	irr::video::ITexture* t = rh->appendTexture(texture, other.texture, pos);
	if (!t) return false;

	texture = t;
	return true;
}

Vec4 Texture::getColor(const Vec2& pos) {
	if (!texture) return Vec4();
	return rh->getColor(texture, pos);
}

bool Texture::setColor(const Vec2& pos, const Vec4& color) {
	if (!texture) return false;
	return rh->setColor(texture, pos, color);
}

bool Texture::key(const Vec4& color) {
	if (!texture) return false;
	return rh->keyColor(texture, color);
}

std::string Texture::getPath() const {
	if (!texture) return "";
	return texture->getName().getPath().c_str();
}

int Texture::getRefCount() {
	return texture ? texture->getReferenceCount() : 0;
}

void Texture::makeRenderTexture(const Vec2& size) {
	r->removeTexture(texture);
	texture = r->createRenderTargetTexture(size, nullptr);
}

#include "Objects/Camera.h"
void Texture::makeRenderTexture(const Vec2& size, const Camera& c) {
	r->removeTexture(texture);
	texture = r->createRenderTargetTexture(size, static_cast<irr::scene::ICameraSceneNode*>(c.getNode()));
}

void Object::TextureBind::bind(lua_State* ls, Renderer* rend) {
	r = rend;
	rh = rend->GetRenderHelper();
	l = ls;

	sol::state_view view(ls);
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
	// Returns boolean
	obj.set_function("write", &Texture::write);

	// Crops the `Texture` to the dimensions provided. Be mindful as cropping creates a new `Texture` in the renderer.
	// Params Vec2 topLeft, Vec2 bottomRight
	// Returns boolean
	obj.set_function("crop", &Texture::crop);

	// Appends another `Texture` onto this `Texture`.
	// Params Texture toAppend, Vec2 pos
	// Returns boolean
	obj.set_function("append", &Texture::append);

	// Returns the color of the pixel at `pos` in this `Texture`.
	// Params Vec2 pos
	// Returns Vec4
	obj.set_function("getColor", &Texture::getColor);

	// Replaces the pixel at `pos` with a pixel of color `color`.
	// Params Vec2 pos, Vec4 color
	// Returns boolean
	obj.set_function("setColor", &Texture::setColor);

	// Removes the color `keyColor` from anywhere in this `Texture`.
	// Params Vec4 keyColor
	// Returns boolean
	obj.set_function("keyColor", &Texture::key);

	// Returns the reference count for this `Texture`.
	// Returns number
	obj.set_function("getReferenceCount", &Texture::getRefCount);

	// Renders the scene to this `Texture`.
	// Params Vec2 size
	// Params Vec2 size, Camera viewpoint
	// Returns void
	obj.set_function("renderToTexture",
		sol::overload(
			sol::resolve<void(const Vec2&)>(&Texture::makeRenderTexture),
			sol::resolve<void(const Vec2&, const Camera&)>(&Texture::makeRenderTexture)
		));

	// Purges this `Texture`, effectively removing it from memory. Objects using this `Texture` will use an engine-defined `Texture` instead, but it is recommended to remove references to this `Texture` first.
	// Returns nil
	obj.set_function("purge", &Texture::purge);

	// End Object
}