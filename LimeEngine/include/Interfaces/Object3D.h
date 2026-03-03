#pragma once
#include <sol/optional.hpp>

class Application;

#include "Objects/Vec3.h"

namespace irr {
	namespace scene {
		class ISceneNode;
	}
}

class Object3D {
public:
    virtual ~Object3D() = default;
	virtual irr::scene::ISceneNode* getNode() const = 0;
	virtual void destroy() = 0;

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

	sol::object i_destroy();
protected:

private:
};

namespace Interface::Object3DBind {
    void bind(Application* app);
}