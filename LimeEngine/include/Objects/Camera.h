#pragma once
#include "Interfaces/SceneObject.h"
#include "Interfaces/Object3D.h"

extern "C" { struct lua_State; }

#include "ICameraSceneNode.h"

class RenderHelper;
class Vec2;
class Vec3;

namespace irr {
	namespace scene {
		class ISceneNode;
		class ICameraSceneNode;
	}
}

class Camera : public SceneObject, public Object3D {
public:
	Camera();
	Camera(const Vec3& pos);
	Camera(const Vec3& pos, const Vec3& rotation);
	irr::scene::ISceneNode* getNode() const override { return camera; }
	void destroy() override;
	void setDebug(bool v) override;

	void setPosition(const Vec3& pos) override;
	void setRotation(const Vec3& rot) override;

	Vec3 getUp() const;
	void setUp(const Vec3& up);

	Vec2 getViewPlanes() const;
	void setViewPlanes(const Vec2& vp);

	float getAspectRatio() const;
	void setAspectRatio(float ar);

	float getFieldOfView() const;
	void setFieldOfView(float fov);

	bool getOrtho() const;
	void setOrtho(bool o);

	Vec3 getForward() const;
	Vec3 getLeft() const;

	void setActive() const;

private:
	irr::scene::ICameraSceneNode* camera = nullptr;
	irr::scene::ISceneNode* left = nullptr;
	irr::scene::ISceneNode* forward = nullptr;
};

namespace Object::CameraBind {
    void bind(lua_State* ls, RenderHelper* renh);
}