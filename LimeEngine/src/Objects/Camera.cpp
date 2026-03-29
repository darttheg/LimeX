#include "Objects/Camera.h"

#include "RenderHelper.h"
#include "Objects/Vec2.h"
#include "Objects/Vec3.h"

#include "irrlicht.h"
#include <sol/sol.hpp>

static RenderHelper* rh;

Camera::Camera() : Camera(Vec3(), Vec3()){
}

Camera::Camera(const Vec3& pos) : Camera(pos, Vec3()) {
}

Camera::Camera(const Vec3& pos, const Vec3& rot) {
	bool setActive = !rh->getActiveCamera();

	camera = rh->createCameraNode();
	if (!camera) return;

	camera->setNearValue(0.1);
	forward = rh->createEmptyNode();
	left = rh->createEmptyNode();
	camera->addChild(forward);
	forward->setPosition(irr::core::vector3df(0, 0, 1));
	camera->addChild(left);
	left->setPosition(irr::core::vector3df(-1, 0, 0));

	setPosition(pos);
	setRotation(rot);
	rh->updateCameraMatrix(camera);

	if (setActive)
		rh->setActiveCamera(camera, left, forward);
}

void Camera::destroy() {
	if (camera) camera->remove();
	camera = nullptr;
}

#include "Objects/DebugAxisPlaneNode.h"
void Camera::setDebug(bool v) {
	if (v) {
		if (dVisual) { dVisual->drop(); dVisual->remove(); }
		dVisual = rh->createDebugNode(DEBUG3D_TYPE::CAMERA);
		dAxis->setPointerLength(1.0f);
	} else {
		if (dVisual) {
			dVisual->drop();
			dVisual->remove();
		}
	}
}

static void updateLookTarget(irr::scene::ICameraSceneNode* c, irr::scene::ISceneNode* forward) {
	if (!c) return;
	c->updateAbsolutePosition();
	forward->updateAbsolutePosition();
	c->setTarget(forward->getAbsolutePosition());
}

void Camera::setPosition(const Vec3& pos) {
	Object3D::setPosition(pos);
	updateLookTarget(camera, forward);
}

void Camera::setRotation(const Vec3& rot) {
	Object3D::setRotation(rot);
	updateLookTarget(camera, forward);
}

Vec3 Camera::getUp() const {
	if (!camera) return Vec3();
	irr::core::vector3df f = camera->getUpVector();
	return Vec3(f.X, f.Y, f.Z);
}

void Camera::setUp(const Vec3& up) {
	if (!camera) return;
	camera->setUpVector(irr::core::vector3df(up.getX(), up.getY(), up.getZ()));
}

Vec2 Camera::getViewPlanes() const {
	if (!camera) return Vec2();
	return Vec2(camera->getNearValue(), camera->getFarValue());
}

void Camera::setViewPlanes(const Vec2& vp) {
	if (!camera) return;
	camera->setNearValue(vp.getX());
	camera->setFarValue(vp.getY());
}

float Camera::getAspectRatio() const {
	if (!camera) return 0.0f;
	return camera->getAspectRatio();
}

void Camera::setAspectRatio(float ar) {
	if (!camera) return;
	camera->setAspectRatio(ar);
}

float Camera::getFieldOfView() const {
	if (!camera) return 0.0f;
	return camera->getFOV() * irr::core::RADTODEG;
}

void Camera::setFieldOfView(float fov) {
	if (!camera) return;
	camera->setFOV(fov * irr::core::DEGTORAD);
}

bool Camera::getOrtho() const {
	if (!camera) return false;
	return camera->isTrulyOrthogonal;
}

void Camera::setOrtho(bool o) {
	if (!camera) return;
	camera->isTrulyOrthogonal = o;
}

Vec3 Camera::getForward() const {
	if (!camera) return Vec3();

	irr::core::vector3df f = (forward->getAbsolutePosition() - camera->getAbsolutePosition()).normalize();
	return Vec3(f.X, f.Y, f.Z);
}

Vec3 Camera::getLeft() const {
	if (!camera) return Vec3();

	irr::core::vector3df f = (left->getAbsolutePosition() - camera->getAbsolutePosition()).normalize();
	return Vec3(f.X, f.Y, f.Z);
}

void Camera::setActive() const {
	if (!camera) return;

	rh->setActiveCamera(camera, left, forward);
}

void Object::CameraBind::bind(lua_State* ls, RenderHelper* renh) {
	rh = renh;

	// Object Camera, A viewpoint in the 3D world.
	// Inherits Object3D

	// Constructor
	// Constructor Vec3 pos
	// Constructor Vec3 pos, Vec3 rot

	sol::state_view view(ls);
	sol::usertype<Camera> obj = view.new_usertype<Camera>(
		"Camera",
		sol::constructors<Camera(), Camera(const Vec3& pos), Camera(const Vec3& pos, const Vec3& rot)>(),

		sol::base_classes, sol::bases<Object3D>(),
		sol::meta_function::type, [](const Camera&) { return "Camera"; },

		// Field Vec3 position, The 3D position of this object in the scene.
		"position", sol::property(
			[](Camera& c) { return Vec3{ [&] { return c.getPosition(); }, [&](auto v) { c.setPosition(v); } }; },
			[](Camera& c, const Vec3& v) { c.setPosition(v); }
		),
		// Field Vec3 rotation, The 3D rotation of this object in the scene in degrees.
		"rotation", sol::property(
			[](Camera& c) { return Vec3{ [&] { return c.getRotation(); }, [&](auto v) { c.setRotation(v); } }; },
			[](Camera& c, const Vec3& v) { c.setRotation(v); }
		),

		// Field Vec3 up, The up vector of this `Camera`.
		"up", sol::property(
			[](Camera& c) { return Vec3{ [&] { return c.getUp(); }, [&](auto v) { c.setUp(v); } }; },
			[](Camera& c, const Vec3& v) { c.setUp(v); }
		),
		// Field Vec2 viewPlanes, The near and far clipping planes of this `Camera`.
		"viewPlanes", sol::property(
			[](Camera& c) { return Vec2{ [&] { return c.getViewPlanes(); }, [&](auto v) { c.setViewPlanes(v); } }; },
			[](Camera& c, const Vec2& v) { c.setViewPlanes(v); }
		),

		// Field number fieldOfView, The field of view of this `Camera` in degrees.
		"fieldOfView", sol::property(&Camera::getFieldOfView, &Camera::setFieldOfView),
		// Field number aspectRatio, The aspect ratio of this `Camera`.
		"aspectRatio", sol::property(&Camera::getAspectRatio, &Camera::setAspectRatio),
		// Field boolean orthogonal, Whether or not this `Camera` renders orthographically or not. (NOTE: If this is true, `aspectRatio` modifies the zoom factor instead.)
		"orthogonal", sol::property(&Camera::getOrtho, &Camera::setOrtho)
	);

	obj[sol::meta_function::to_string] = [](const Camera& v) {
		return "Camera";
		};

	// Returns the forward vector.
	// Returns Vec3
	obj.set_function("getForward", &Camera::getForward);

	// Returns the left vector.
	// Returns Vec3
	obj.set_function("getLeft", &Camera::getLeft);

	// When rendering, this `Camera` will take over as the active rendering viewpoint.
	// Returns void
	obj.set_function("setActive", &Camera::setActive);

	// End Object
}