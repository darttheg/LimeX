#pragma once

#include "Interfaces/Object2D.h"

class Vec2;

namespace irr {
	namespace gui {
		class IGUIElement;
		class CGUIColoredText;
	}
}

class Text2D : public Object2D {
public:
	Text2D();
	Text2D(const std::string& tx);
	Text2D(const Vec2& pos, const Vec2& sz);

	std::string getText();
	void setText(const std::string& tx);

	int getOpacity();
	void setOpacity(int o);
	void setAlignment(int all);
	void setAlignment(int h, int v);
	void setWordWrap(bool enable);

	bool setFont(const std::string& f);

	irr::gui::IGUIElement* getNode() const override;
private:
	irr::gui::CGUIColoredText* text = nullptr;
};

namespace Object::Text2DBind {
	void bind(Application* app);
}