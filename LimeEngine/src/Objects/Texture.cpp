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
	if (!rh->guardRenderingCheck()) return;
	// texture = rh->createTexture(1, 1, "blank");
	texture = nullptr;
}

Texture::Texture(const std::string& path) : Texture() {
	texture = rh->createTexture(path);
}

Texture::Texture(irr::video::ITexture* tex) : Texture() {
	texture = tex;
}

Texture::Texture(const Vec2& wh) : Texture(wh, "") {
}

Texture::Texture(const Vec2& wh, const std::string& name) : Texture() {
	texture = rh->createTexture(wh.getX(), wh.getY(), name);
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
	return r->writeTextureToPath(texture, outPath);
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

bool Texture::setColor(const Vec2& tl, const Vec2& br, const Vec4& color) {
	if (!texture) return false;
	return rh->setColorRect(texture, tl, br, color);
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

std::string Texture::makeRenderTexture(const Vec2& size) {
	r->removeTexture(texture);
	texture = r->createRenderTargetTexture(size, nullptr);
	return texture ? texture->getName().getPath().c_str() : "";
}

#include "Objects/Camera.h"
std::string Texture::makeRenderTexture(const Vec2& size, const Camera& c) {
	r->removeTexture(texture);
	texture = r->createRenderTargetTexture(size, static_cast<irr::scene::ICameraSceneNode*>(c.getNode()));
	return texture ? texture->getName().getPath().c_str() : "";
}

void Texture::collected() {
	if (texture) r->warnGarbageCollection(texture->getName().getPath().c_str());
}

void Object::TextureBind::bind(lua_State* ls, Renderer* rend) {
	r = rend;
	rh = rend->GetRenderHelper();
	l = ls;

	sol::state_view view(ls);
	sol::usertype<Texture> obj = view.new_usertype<Texture>(
		"Texture",
		sol::constructors<Texture(), Texture(const std::string&), Texture(const Vec2&), Texture(const Vec2&, const std::string&)>(),
		sol::meta_function::type, [](const Texture&) { return "Texture"; },
		sol::meta_function::garbage_collect, [](Texture& t) { t.collected(); }
	);

	obj[sol::meta_function::to_string] = [](const Texture& v) {
		return v.getPath();
		};

	// Object Texture, A texture that is the foundation for all images for 2D and 3D objects.

	// Constructor
	// Constructor Vec2 size, string? name
	// Constructor string path

	// Returns the path of this `Texture`.
	// Returns string
	obj.set_function("getPath", &Texture::getPath);

	// Returns the size of this `Texture`.
	// Returns Vec2
	obj.set_function("getSize", &Texture::getSize);

	// Writes this `Texture` to `path`.
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

	// Sets pixel `color`.
	// Params Vec2 pos, Vec4 color
	// Params Vec2 topLeft, Vec2 bottomRight, Vec4 fillColor
	// Returns boolean
	obj.set_function("setColor",
		sol::overload(
			sol::resolve<bool(const Vec2&, const Vec4&)>(&Texture::setColor),
			sol::resolve<bool(const Vec2&, const Vec2&, const Vec4&)>(&Texture::setColor)
		));

	// Removes the color `keyColor` from anywhere in this `Texture`.
	// Params Vec4 keyColor
	// Returns boolean
	obj.set_function("keyColor", &Texture::key);

	// Returns the reference count for this `Texture`.
	// Returns number
	obj.set_function("getReferenceCount", &Texture::getRefCount);

	// Renders the scene to this `Texture`. Returns the name of this `Texture`.
	// Params Vec2 size
	// Params Vec2 size, Camera viewpoint
	// Returns string
	obj.set_function("renderToTexture",
		sol::overload(
			sol::resolve<std::string(const Vec2&)>(&Texture::makeRenderTexture),
			sol::resolve<std::string(const Vec2&, const Camera&)>(&Texture::makeRenderTexture)
		));

	// Purges this `Texture`, effectively removing it from memory. Objects using this `Texture` will use an engine-defined `Texture` instead, but it is recommended to remove references to this `Texture` first.
	// Returns nil
	obj.set_function("purge", &Texture::purge);

	// End Object
}