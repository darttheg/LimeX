#include "Interfaces/Object3D.h"
#include "Application.h"
#include "Objects/Vec3.h"

#include "irrlicht.h"
using namespace irr;
using namespace scene;

Vec3 Object3D::getPosition() const {
    return position;
}

void Object3D::setPosition(const Vec3& pos) {
    position = pos;
}

Vec3 Object3D::getRotation() const {
    return rotation;
}

void Object3D::setRotation(const Vec3& rot) {
    rotation = rot;
}

Vec3 Object3D::getScale() const {
    return scale;
}

void Object3D::setScale(const Vec3& s) {
    scale = s;
}

bool Object3D::getVisibility() const {
    return visible;
}

void Object3D::setVisibility(bool v) {
    visible = v;
}

bool Object3D::setParent(sol::optional<Object3D*> parent) {
    if (!getNode() || !(*parent)->getNode()) return false;

    getNode()->setParent((*parent)->getNode());
    return true;
}

int Object3D::getID() const {
    return id;
}

void Object3D::setID(int d) {
    id = d;
}

void Interface::Object3DBind::bind(Application* a) {
	sol::state_view view(a->GetLuaState());
	sol::usertype<Object3D> obj = view.new_usertype<Object3D>(
		"Object3D",

        "position", sol::property(
            [](Object3D& c) { return Vec3{ [&] { return c.getPosition(); }, [&](auto v) { c.setPosition(v); } }; },
            [](Object3D& c, const Vec3& v) { c.setPosition(v); }
            ),
        "rotation", sol::property(
            [](Object3D& c) { return Vec3{ [&] { return c.getRotation(); }, [&](auto v) { c.setRotation(v); } }; },
            [](Object3D& c, const Vec3& v) { c.setRotation(v); }
            ),
        "scale", sol::property(
            [](Object3D& c) { return Vec3{ [&] { return c.getScale(); }, [&](auto v) { c.setScale(v); } }; },
            [](Object3D& c, const Vec3& v) { c.setScale(v); }
            ),

        "visible", sol::property(&Object3D::getVisibility, &Object3D::setVisibility),
        "id", sol::property(&Object3D::getID, &Object3D::setID)
	);
}
