#pragma once
#include <sol/optional.hpp>
#include <sol/forward.hpp>

class Application;
class Vec2;
class Vec4;
class Event;

namespace irr {
	namespace gui {
		class IGUIElement;
		class IGUIStaticText;
		class IGUIButton;
	}
}

class Object2D {
public:
	virtual ~Object2D() = default;
	virtual irr::gui::IGUIElement* getNode() const = 0;
	irr::gui::IGUIElement* getButton() const;
	virtual void destroy() = 0;
	sol::object i_destroy();

	Vec2 getPosition() const;
	virtual void setPosition(const Vec2& pos);
	bool getVisibility() const;
	void setVisibility(bool v);
	Vec2 getSize() const;
	void setSize(const Vec2& size);

	bool getBorder() const;
	void setBorder(bool enable);
	Vec4 getBackgroundColor() const;
	void setBackgroundColor(const Vec4& c);

	bool bringToFront() const;
	bool sendToBack() const;
	int getZOrder() const;
	void setZOrder(int z);
	void updateBackgroundZ();

	int getRefCount() const;

	bool parentTo(sol::optional<Object2D*> parent);

	void createButton();
	void removeButton();
	void checkButtonState();
	bool isHovered() const;
	bool hovered = false;
	bool pressed = false;
	void createEvents();
	std::shared_ptr<Event> onPressed = nullptr;
	std::shared_ptr<Event> onHovered = nullptr;

	std::shared_ptr<Event> getHoverEvent() { return onHovered; }
	std::shared_ptr<Event> getPressedEvent() { return onPressed; }
	void setHoverEvent(std::shared_ptr<Event> e);
	void setPressedEvent(std::shared_ptr<Event> e);

	bool getEnabled() const;
	void setEnabled(bool v);
protected:
	irr::gui::IGUIStaticText* bgBorder = nullptr;
	void updateBorderDimensions(const Vec2& sz);
	void updateButtonDimensions();

	irr::gui::IGUIButton* button = nullptr;
private:
	struct Vec4S { int x, y, z, w; };
	Vec4S backgroundColor{ 0, 0, 0, 0 };
	bool hasBorder = false;
	bool hasBackground = false;
	bool btnEnabled = true; // If button is active, this toggles its enabled state

	void setBGBorder();
};

namespace Interface::Object2DBind {
	void bind(Application* app);
}