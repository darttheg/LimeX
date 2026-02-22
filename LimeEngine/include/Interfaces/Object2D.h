#pragma once
#include <sol/optional.hpp>

class Application;

#include "Objects/Vec2.h"

namespace irr {
	namespace gui {
		class IGUIElement;
	}
}

class Object2D {
public:
	virtual ~Object2D() = default;
	virtual irr::gui::IGUIElement* getNode() const = 0;
	// virtual void destroy() = 0;

	Vec2 getPosition() const;
	virtual void setPosition(const Vec2& pos);
	bool getVisibility() const;
	void setVisibility(bool v);
	Vec2 getSize() const;
	void setSize(const Vec2& size);

	bool parentTo(sol::optional<Object2D*> parent);
protected:

private:
};

namespace Interface::Object2DBind {
	void bind(Application* app);
}