#include "Objects/Camera.h"

#include "Application.h"
#include "Renderer.h"
#include "DebugConsole.h"
#include "Interfaces/Object3D.h"
#include "Objects/Vec2.h"
#include "Objects/Vec3.h"

#include "irrlicht.h"

static DebugConsole* d;
static Renderer* r;

Camera::Camera() : Camera(Vec3(), Vec3()){
}

Camera::Camera(const Vec3& pos) : Camera(pos, Vec3()) {
}

Camera::Camera(const Vec3& pos, const Vec3& rot) {
	camera = r->createCameraNode();
	if (!camera) return;

	forward = r->createEmptyNode();
	left = r->createEmptyNode();
	camera->addChild(forward);
	forward->setPosition(irr::core::vector3df(0, 0, 1));
	camera->addChild(left);
	left->setPosition(irr::core::vector3df(-1, 0, 0));

	setPosition(pos);
	setRotation(rot);
	r->updateCameraMatrix(camera);
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

void Object::CameraBind::bind(Application* a) {
	r = a->GetRenderer();
	d = a->GetDebugConsole();

	// Object Camera, A viewpoint in the 3D world.
	// Inherits Object3D

	// Constructor
	// Constructor Vec3 pos
	// Constructor Vec3 pos, Vec3 rot

	sol::state_view view(a->GetLuaState());
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

		// Field Vec3 up, The up vector of this Camera.
		"up", sol::property(
			[](Camera& c) { return Vec3{ [&] { return c.getUp(); }, [&](auto v) { c.setUp(v); } }; },
			[](Camera& c, const Vec3& v) { c.setUp(v); }
		),
		// Field Vec2 viewPlanes, The near and far clipping planes of this Camera.
		"viewPlanes", sol::property(
			[](Camera& c) { return Vec2{ [&] { return c.getViewPlanes(); }, [&](auto v) { c.setViewPlanes(v); } }; },
			[](Camera& c, const Vec2& v) { c.setViewPlanes(v); }
		),

		// Field number fieldOfView, The field of view of this Camera in degrees.
		"fieldOfView", sol::property(&Camera::getFieldOfView, &Camera::setFieldOfView),
		// Field number aspectRatio, The aspect ratio of this Camera.
		"aspectRatio", sol::property(&Camera::getAspectRatio, &Camera::setAspectRatio),
		// Field boolean orthogonal, Whether or not this Camera renders orthographically or not. (NOTE: If this is true, aspectRatio modifies the zoom factor instead.)
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

	// End Object
}