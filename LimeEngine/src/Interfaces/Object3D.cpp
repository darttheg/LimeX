#include "Interfaces/Object3D.h"
#include "Application.h"
#include "Objects/Vec3.h"

#include "irrlicht.h"
using namespace irr;
using namespace scene;

Vec3 Object3D::getPosition() const {
    if (!getNode()) return Vec3();
    return Vec3(getNode()->getPosition().X, getNode()->getPosition().Y, getNode()->getPosition().Z);
}

void Object3D::setPosition(const Vec3& pos) {
    if (!getNode()) return;
    getNode()->setPosition(irr::core::vector3df(pos.getX(), pos.getY(), pos.getZ()));
}

Vec3 Object3D::getRotation() const {
    if (!getNode()) return Vec3();
    return Vec3(getNode()->getRotation().X, getNode()->getRotation().Y, getNode()->getRotation().Z);
}

void Object3D::setRotation(const Vec3& rot) {
    if (!getNode()) return;
    getNode()->setRotation(irr::core::vector3df(rot.getX(), rot.getY(), rot.getZ()));
}

Vec3 Object3D::getScale() const {
    if (!getNode()) return Vec3();
    return Vec3(getNode()->getScale().X, getNode()->getScale().Y, getNode()->getScale().Z);
}

void Object3D::setScale(const Vec3& s) {
    if (!getNode()) return;
    getNode()->setScale(irr::core::vector3df(s.getX(), s.getY(), s.getZ()));
}

bool Object3D::getVisibility() const {
    return getNode() ? getNode()->isVisible() : false;
}

void Object3D::setVisibility(bool v) {
    if (!getNode()) return;
    getNode()->setVisible(v);
}

bool Object3D::parentTo(sol::optional<Object3D*> parent) {
    if (!getNode() || !(*parent)->getNode()) return false;

    getNode()->setParent((*parent)->getNode());
    return true;
}

int Object3D::getID() const {
    return getNode() ? getNode()->getID() : 0;
}

void Object3D::setID(int d) {
    if (!getNode()) return;
    getNode()->setID(d);
}

void Interface::Object3DBind::bind(Application* a) {
	sol::state_view view(a->GetLuaState());

    // Interface Object3D

	sol::usertype<Object3D> obj = view.new_usertype<Object3D>(
		"Object3D",

        // Field Vec3 position, The 3D position of this object in the scene.
        "position", sol::property(
            [](Object3D& c) { return Vec3{ [&] { return c.getPosition(); }, [&](auto v) { c.setPosition(v); } }; },
            [](Object3D& c, const Vec3& v) { c.setPosition(v); }
            ),
        // Field Vec3 rotation, The 3D rotation of this object in the scene in degrees.
        "rotation", sol::property(
            [](Object3D& c) { return Vec3{ [&] { return c.getRotation(); }, [&](auto v) { c.setRotation(v); } }; },
            [](Object3D& c, const Vec3& v) { c.setRotation(v); }
            ),
        // Field Vec3 scale, The 3D scale of this object in the scene.
        "scale", sol::property(
            [](Object3D& c) { return Vec3{ [&] { return c.getScale(); }, [&](auto v) { c.setScale(v); } }; },
            [](Object3D& c, const Vec3& v) { c.setScale(v); }
            ),

        // Field boolean visible, Determines the visibility of this object and its children.
        "visible", sol::property(&Object3D::getVisibility, &Object3D::setVisibility),

        // Field number id, The identifier for this object to be used in raycasts and object selection.
        "id", sol::property(&Object3D::getID, &Object3D::setID)
	);

    // Parents this object to another 3D object.
    // Params any child
    // Returns void
    obj.set_function("parentTo", &Object3D::parentTo);

    // End Interface
}
