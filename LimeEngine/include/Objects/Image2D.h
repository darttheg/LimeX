#pragma once

#include "Interfaces/Object2D.h"

class Vec2;
class Texture;

namespace irr {
	namespace gui {
		class IGUIElement;
		class IGUIImage;
	}
}

class Image2D : public Object2D {
public:
	Image2D();
	Image2D(const Texture& t);
	Image2D(const Vec2& pos, const Vec2& sz);

	bool loadTexture(const Texture& tx);

	irr::gui::IGUIElement* getNode() const override;
private:
	irr::gui::IGUIImage* img = nullptr;
};

namespace Object::Image2DBind {
	void bind(Application* app);
}