#include "Interfaces/Object3D.h"
#include "Application.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"

#include "irrlicht.h"
using namespace irr;
using namespace scene;

static Application* a = nullptr;

Vec3 Object3D::getPosition() const {
    if (!getNode()) return Vec3();
    return Vec3(getNode()->getPosition().X, getNode()->getPosition().Y, getNode()->getPosition().Z);
}

void Object3D::setPosition(const Vec3& pos) {
    if (!getNode()) return;
    getNode()->setPosition(irr::core::vector3df(pos.getX(), pos.getY(), pos.getZ()));

    if (dVisual)
        dVisual->setPosition(getNode()->getAbsolutePosition());
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

int Object3D::getRefCount() const {
    return getNode() ? getNode()->getReferenceCount() : -1;
}

bool Object3D::parentTo(sol::optional<Object3D*> parent) {
    if (!getNode()) return false;

    if (!parent || *parent == nullptr) {
        getNode()->setParent(getNode()->getSceneManager()->getRootSceneNode());
        return true;
    }

    Object3D* p = *parent;
    if (!p->getNode()) return false;
    getNode()->setParent(p->getNode());
    return true;
}

void Object3D::i_setDebug(bool v) {
    if (debug == v) return;
    setDebug(v);
    debug = v;

    if (!getNode()) return;
    getNode()->updateAbsolutePosition();
    dVisual->setPosition(getNode()->getPosition());
}

sol::object Object3D::i_destroy() {
    setDebug(false);
    debug = false;
    destroy();
    return sol::make_object(a->GetLuaState(), sol::nil);
}

int Object3D::getID() const {
    return getNode() ? getNode()->getID() : 0;
}

void Object3D::setID(int d) {
    if (!getNode()) return;
    getNode()->setID(d);
}

Vec4 Object3D::getBoundingBox() const {
    if (!getNode()) return Vec4();

    irr::core::aabbox3df b = getNode()->getBoundingBox();
    return Vec4(b.MinEdge.X, b.MinEdge.Y, b.MaxEdge.X, b.MaxEdge.Y);
}

bool Object3D::isPointInside(const Vec3& pos) const {
    if (!getNode()) return false;

    irr::core::aabbox3df b = getNode()->getBoundingBox();
    return b.isPointInside(irr::core::vector3df(pos.getX(), pos.getY(), pos.getZ()));
}

void Interface::Object3DBind::bind(Application* app) {
    a = app;
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
        "id", sol::property(&Object3D::getID, &Object3D::setID),

        // Field boolean debug, Show debug information about this object in the scene.
        "debug", sol::property(&Object3D::getDebug, &Object3D::i_setDebug)
	);
     
    // Parents this object to another 3D object.
    // Params any parent
    // Returns void
    obj.set_function("parentTo", &Object3D::parentTo);

    // Returns the reference count for this object.
    // Returns number
    obj.set_function("getReferenceCount", &Object3D::getRefCount);

    // Destroys this object.
    // Returns nil
    obj.set_function("destroy", &Object3D::i_destroy);

    // Returns the bounding box of this object, following: (MinEdgeX, MinEdgeY, MaxEdgeX, MaxEdgeY).
    // Returns Vec4
    obj.set_function("getBoundingBox", &Object3D::getBoundingBox);

    // Returns true if `pos` is inside this object's bounding box.
    // Params Vec3 pos
    // Returns boolean
    obj.set_function("isPointInside", &Object3D::isPointInside);

    // End Interface
}
