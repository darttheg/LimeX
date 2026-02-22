#include "Interfaces/Object2D.h"
#include "Application.h"
#include "Objects/Vec3.h"

#include "irrlicht.h"
using namespace irr;
using namespace scene;

Vec2 Object2D::getPosition() const {
    if (!getNode()) return Vec2();
    
    const auto r = getNode()->getRelativePosition();
    return Vec2(r.UpperLeftCorner.X, r.UpperLeftCorner.Y);
}

void Object2D::setPosition(const Vec2& pos) {
    if (!getNode()) return;

    auto r = getNode()->getRelativePosition();
    int x = pos.getX();
    int y = pos.getY();
    int w = r.getWidth();
    int h = r.getHeight();

    r.UpperLeftCorner.X = x;
    r.UpperLeftCorner.Y = y;
    r.LowerRightCorner.X = x + w;
    r.LowerRightCorner.Y = y + h;

    getNode()->setRelativePosition(r);
}

bool Object2D::getVisibility() const {
    return getNode() ? getNode()->isVisible() : false;
}

void Object2D::setVisibility(bool v) {
    if (!getNode()) return;
    getNode()->setVisible(v);
}

Vec2 Object2D::getSize() const {
    if (!getNode()) return Vec2();

    irr::core::recti r = getNode()->getRelativePosition();
    return Vec2(r.LowerRightCorner.X - r.UpperLeftCorner.X, r.LowerRightCorner.Y - r.UpperLeftCorner.Y);
}

void Object2D::setSize(const Vec2& size) {
    if (!getNode()) return;

    irr::core::recti r = getNode()->getRelativePosition();
    r.LowerRightCorner.X = r.UpperLeftCorner.X + size.getX();
    r.LowerRightCorner.Y = r.UpperLeftCorner.Y + size.getY();
    getNode()->setRelativePosition(r);
}

bool Object2D::parentTo(sol::optional<Object2D*> parent) {
    if (!getNode() || !(*parent)->getNode()) return false;

    (*parent)->getNode()->addChild(getNode());
    return true;
}

void Interface::Object2DBind::bind(Application* a) {
    sol::state_view view(a->GetLuaState());

    // Interface Object2D

    sol::usertype<Object2D> obj = view.new_usertype<Object2D>(
        "Object2D",

        // Field Vec2 position, The 2D position of this object on the screen.
        "position", sol::property(
            [](Object2D& c) { return Vec2{ [&] { return c.getPosition(); }, [&](auto v) { c.setPosition(v); } }; },
            [](Object2D& c, const Vec2& v) { c.setPosition(v); }
        ),

        // Field Vec2 size, The 2D size of this object.
        "size", sol::property(
            [](Object2D& c) { return Vec2{ [&] { return c.getSize(); }, [&](auto v) { c.setSize(v); } }; },
            [](Object2D& c, const Vec2& v) { c.setSize(v); }
        ),

        // Field boolean visible, Determines the visibility of this object and its children.
        "visible", sol::property(&Object2D::getVisibility, &Object2D::setVisibility)
    );

    // Parents this object to another 2D object.
    // Params any child
    // Returns void
    obj.set_function("parentTo", &Object2D::parentTo);

    // End Interface
}