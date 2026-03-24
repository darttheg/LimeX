#pragma once

#include "Interfaces/Object2D.h"

extern "C" { struct lua_State; }
class RenderHelper;
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
	void destroy() override;

	bool loadTexture(const Texture& tx);
	bool getScaledFit() const;
	void setScaledFit(bool v);

	irr::gui::IGUIElement* getNode() const override;
private:
	irr::gui::IGUIImage* img = nullptr;
};

namespace Object::Image2DBind {
	void bind(lua_State* ls, RenderHelper* renh);
}