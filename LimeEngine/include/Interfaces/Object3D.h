#pragma once
#include <sol/optional.hpp>
#include <sol/forward.hpp>

extern "C" { struct lua_State; }
class RenderHelper;
class Vec3;
class Vec4;
class DebugAxisPlaneNode;
// #include "Objects/Vec3.h"

namespace irr {
	namespace scene {
		class ISceneNode;
		class IBillboardSceneNode;
	}
}

class Object3D {
public:
    virtual ~Object3D() = default;
	virtual irr::scene::ISceneNode* getNode() const = 0;
	virtual void destroy() = 0;
	virtual void setDebug(bool v) = 0;

	Vec3 getPosition() const;
	virtual void setPosition(const Vec3& pos);
	Vec3 getRotation() const;
	virtual void setRotation(const Vec3& rot);
	Vec3 getScale() const;
	void setScale(const Vec3& s);

	bool getVisibility() const;
	void setVisibility(bool v);

	int getRefCount() const;

	int getID() const;
	void setID(int d);

	bool parentTo(sol::optional<Object3D*> parent);
	bool hasParent();

	Vec4 getBoundingBox() const;
	bool isPointInside(const Vec3& pos) const;

	bool getDebug() const { return debug; }
	void i_setDebug(bool v);

	sol::object i_destroy();

	void setAttribute(sol::object key, sol::object value);
	sol::object getAttribute(sol::object key);
	sol::object getAttributes();
	void clearAttributes();
protected:
	bool debug = false;
	irr::scene::IBillboardSceneNode* dVisual = nullptr;
	DebugAxisPlaneNode* dAxis = nullptr;
private:
};

namespace Interface::Object3DBind {
    void bind(lua_State* ls, RenderHelper* renh);
}