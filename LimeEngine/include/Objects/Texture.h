#pragma once
#include <string>

extern "C" { struct lua_State; }

class Renderer;

class Camera;
class Vec2;
class Vec4;

namespace irr {
	namespace video {
		class ITexture;
	}
}

#include "sol/forward.hpp"

class Texture {
public:
	Texture();
	Texture(int w, int h, const std::string& name = "");
	Texture(const std::string& path);
	Texture(irr::video::ITexture* tex);
	sol::object purge();

	Vec2 getSize();
	bool write(const std::string& outPath);
	bool crop(const Vec2& tl, const Vec2& br);
	bool append(const Texture& other, const Vec2& pos);
	Vec4 getColor(const Vec2& pos);
	bool setColor(const Vec2& pos, const Vec4& color);
	bool key(const Vec4& color);
	std::string getPath() const;
	int getRefCount();

	std::string makeRenderTexture(const Vec2& size);
	std::string makeRenderTexture(const Vec2& size, const Camera& c);

	void collected();

	irr::video::ITexture* getTexture() const { return texture; }
private:
	irr::video::ITexture* texture = nullptr;
};

namespace Object::TextureBind {
	void bind(lua_State* ls, Renderer* rend);
}