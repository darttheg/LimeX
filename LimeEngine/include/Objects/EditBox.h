#pragma once

#include "Interfaces/Object2D.h"

extern "C" { struct lua_State; }
class RenderHelper;
class GUIManager;
class Vec2;

namespace irr {
	namespace gui {
		class IGUIElement;
		class CGUIColoredText;
		class IGUIEditBox;
	}
}

class EditBox : public Object2D {
public:
	EditBox();
	EditBox(const std::string& tx);
	EditBox(const Vec2& pos, const Vec2& sz);
	void destroy() override;

	std::string getText();
	void setText(const std::string& tx);

	bool setFont(const std::string& f);

	void setAlignment(int all);
	void setAlignment(int h, int v);

	bool getWordWrap();
	void setWordWrap(bool enable);

	bool getIsPassword();
	void setIsPassword(bool enable);

	bool getIsEnabled();
	void setIsEnabled(bool enable);

	bool getMultiLine();
	void setMultiLine(bool enable);

	bool getAutoScroll();
	void setAutoScroll(bool enable);

	bool getFocused();
	void setFocused(bool v);

	int getMaxCharCount() const;
	void setMaxCharCount(int v);

	int getPos();
	void setPos(int v);

	bool getIsDrawingBackground();
	void setIsDrawingBackground(bool v);

	irr::gui::IGUIElement* getNode() const override;
private:
	bool wraps = false;
	bool bg = true;
	irr::gui::IGUIEditBox* ebox = nullptr;
};

namespace Object::EditBoxBind {
	void bind(lua_State* ls, RenderHelper* renh, GUIManager* gu);
}