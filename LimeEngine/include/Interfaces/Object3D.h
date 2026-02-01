#pragma once
#include <sol/optional.hpp>

class Application;
class Vec3;

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
	void setPosition(const Vec3& pos);
	Vec3 getRotation() const;
	void setRotation(const Vec3& rot);
	Vec3 getScale() const;
	void setScale(const Vec3& s);

	bool getVisibility() const;
	void setVisibility(bool v);

	int getID() const;
	void setID(int d);

	bool setParent(sol::optional<Object3D*> parent);
protected:

private:
	Vec3 position;
	Vec3 rotation;
	Vec3 scale;
	bool visible = true;
	int id = 0;

	bool dirty = false;
};

namespace Interface::Object3DBind {
    void bind(Application* app);
}