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
	namespace scene {
		class ICameraSceneNode;
	}
}

#include "sol/forward.hpp"

class Texture {
public:
	Texture();
	Texture(const Vec2& wh);
	Texture(const Vec2& wh, const std::string& name);
	Texture(const std::string& path);
	Texture(irr::video::ITexture* tex);
	sol::object purge();

	Vec2 getSize();
	bool write(const std::string& outPath);
	bool crop(const Vec2& tl, const Vec2& br);
	bool append(const Texture& other, const Vec2& pos);
	Vec4 getColor(const Vec2& pos);
	bool setColor(const Vec2& pos, const Vec4& color);
	bool setColor(const Vec2& tl, const Vec2& br, const Vec4& color);
	void clear();
	void clear(const Vec4& color);
	bool key(const Vec4& color);
	std::string getPath() const;
	int getRefCount();

	Texture toNineSlice(int cornerMargin, const Vec2& size);
	Texture toNineSlice(int cornerMargin, const Vec2& size, const std::string& name);

	std::string makeRenderTexture(const Vec2& size, const std::string& name = "");
	std::string makeRenderTexture(const Vec2& size, const Camera& c, const std::string& name = "");

	void collected();

	irr::video::ITexture* getTexture() const { return texture; }
private:
	std::string renderLive(const Vec2& size, irr::scene::ICameraSceneNode* cam, const std::string& name);

	irr::video::ITexture* texture = nullptr;
	bool live = false;
};

namespace Object::TextureBind {
	void bind(lua_State* ls, Renderer* rend);
}