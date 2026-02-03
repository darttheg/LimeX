#pragma once
#include <string>

class Application;

class Vec2;
class Vec4;

namespace irr {
	namespace video {
		class ITexture;
	}
}

class Image {
public:
	Image();
	Image(int w, int h, const std::string& name = "");
	Image(const std::string& path);

	Vec2 getSize();
	void write(const std::string& outPath);
	void crop(const Vec2& tl, const Vec2& br);
	void append(const Image& other, const Vec2& pos);
	Vec4 getColor(const Vec2& pos);
	void setColor(const Vec2& pos, const Vec4& color);
	void key(const Vec4& color);
	std::string getPath() const;
private:
	irr::video::ITexture* texture = nullptr;
};

namespace Object::ImageBind {
	void bind(Application* app);
}