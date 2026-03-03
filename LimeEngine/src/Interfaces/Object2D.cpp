#include "Interfaces/Object2D.h"
#include "Objects/Event.h"
#include "Application.h"
#include "Renderer.h"
#include "Objects/Vec2.h"
#include "Objects/Vec4.h"

#include "irrlicht.h"
using namespace irr;
using namespace scene;

static Application* a = nullptr;
static Renderer* r = nullptr;

irr::gui::IGUIElement* Object2D::getButton() const {
    return button;
}

Vec2 Object2D::getPosition() const {
    if (!getNode()) return Vec2();
    
    const auto r = getNode()->getRelativePosition();
    return Vec2(r.UpperLeftCorner.X, r.UpperLeftCorner.Y);
}

void Object2D::setPosition(const Vec2& pos) {
    if (!getNode()) return;
    getNode()->setRelativePosition(irr::core::position2di(pos.getX(), pos.getY()));
    if (bgBorder) bgBorder->setRelativePosition(irr::core::position2di(pos.getX(), pos.getY()));
}

bool Object2D::getVisibility() const {
    return getNode() ? getNode()->isVisible() : false;
}

void Object2D::setVisibility(bool v) {
    if (!getNode()) return;
    getNode()->setVisible(v);
    if (bgBorder) bgBorder->setVisible(v);
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
    updateBorderDimensions(size);
    updateButtonDimensions();
}

bool Object2D::getBorder() const {
    return getNode() && bgBorder;
}

void Object2D::setBorder(bool enable) {
    if (!getNode()) return;

    hasBorder = enable;
    setBGBorder();
    if (bgBorder)
        bgBorder->setDrawBorder(hasBorder);
}

Vec4 Object2D::getBackgroundColor() const {
    return Vec4(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
}

void Object2D::setBackgroundColor(const Vec4& c) {
    if (!getNode()) return;

    backgroundColor.x = c.getX();
    backgroundColor.y = c.getY();
    backgroundColor.z = c.getZ();
    backgroundColor.w = c.getW();

    hasBackground = backgroundColor.w > 0;
    setBGBorder();
    if (bgBorder)
        bgBorder->setBackgroundColor(irr::video::SColor(backgroundColor.w, backgroundColor.x, backgroundColor.y, backgroundColor.z));
}

bool Object2D::bringToFront() const {
    if (!getNode()) return false;
    return getNode()->getParent()->bringToFront(getNode());
}

bool Object2D::sendToBack() const {
    if (!getNode()) return false;
    return getNode()->getParent()->sendToBack(getNode());
}

int Object2D::getZOrder() const {
    return getNode() ? getNode()->getZOrder() : 0;
}

void Object2D::setZOrder(int z) {
    if (!getNode()) return;
    getNode()->setZOrder(z); // Hmmm... is this legit? No. It doesn't work.
}

void Object2D::updateBackgroundZ() {
    if (!getNode() || bgBorder) return;
    getNode()->sendToBack(bgBorder);
}

bool Object2D::parentTo(sol::optional<Object2D*> parent) {
    if (!getNode() || !(*parent)->getNode()) return false;

    (*parent)->getNode()->addChild(getNode());
    if (bgBorder) (*parent)->getNode()->addChild(bgBorder);
    return true;
}

void Object2D::setHoverEvent(std::shared_ptr<Event> e) {
    onHovered = std::move(e);
}

void Object2D::setPressedEvent(std::shared_ptr<Event> e) {
    onPressed = std::move(e);
}

void Object2D::updateBorderDimensions(const Vec2& sz) {
    if (!getNode() || !bgBorder) return;

    irr::core::recti r;
    r.LowerRightCorner.X = r.UpperLeftCorner.X + sz.getX();
    r.LowerRightCorner.Y = r.UpperLeftCorner.Y + sz.getY();

    bgBorder->setRelativePosition(r);
}

void Object2D::updateButtonDimensions() {
    if (!button) return;
    irr::core::recti rect = getNode()->getRelativePosition();
    button->setRelativePosition(rect);
}

void Object2D::setBGBorder() {
    if (!getNode()) return;

    if ((hasBorder || hasBackground) && !bgBorder) {
        bgBorder = r->createStaticText();
        bgBorder->setVisible(getNode()->isVisible());
        getNode()->getParent()->addChild(bgBorder);
        getNode()->getParent()->sendToBack(bgBorder);
        bgBorder->setDrawBackground(true);
        bgBorder->setBackgroundColor(irr::video::SColor(backgroundColor.w, backgroundColor.x, backgroundColor.y, backgroundColor.z));
        updateBorderDimensions(getSize());
        bgBorder->setRelativePosition(irr::core::position2di(getPosition().getX(), getPosition().getY()));
    } else if (!(hasBorder || hasBackground) && bgBorder) {
        bgBorder->remove();
    }
}

bool Object2D::getEnabled() const {
    return getNode() && button && btnEnabled;
}

void Object2D::setEnabled(bool v) {
    if (!getNode() || !button) return;
    button->setEnabled(v);
}

void Object2D::createButton() {
    if (button || !getNode()) return;
    button = r->createButton();
    updateButtonDimensions();
    button->setEnabled(btnEnabled);
    // button->setVisible(false);

    r->addButtonPair(*this);
}

void Object2D::removeButton() {
    if (!button) return;
    r->removeButtonPair(*this);
    button->remove();
}

void Object2D::checkButtonState() {
    int sz = onHovered->getSize() + onPressed->getSize();
    if (!button && sz > 0)
        createButton();
    else if (button && sz == 0)
        removeButton();
}

bool Object2D::isHovered() const {
    return r->isElementHovered(*this);
}

void Object2D::createEvents() {
    onHovered = std::make_shared<Event>();
    onPressed = std::make_shared<Event>();

    onHovered->setOnLengthChanged([this]() {
        checkButtonState();
    });

    onPressed->setOnLengthChanged([this]() {
        checkButtonState();
    });
}

sol::object Object2D::i_destroy() {
    removeButton();
    setBorder(false);
    destroy();
    return sol::make_object(a->GetLuaState(), sol::nil);
}

void Interface::Object2DBind::bind(Application* app) {
    a = app;
    r = a->GetRenderer();
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
        "visible", sol::property(&Object2D::getVisibility, &Object2D::setVisibility),

        // Field boolean border, Displays a border outlining this object's bounding box on the screen.
        "border", sol::property(&Object2D::getBorder, &Object2D::setBorder),

        // Field Vec4 backgroundColor, The RGBA background color of this object.
        "backgroundColor", sol::property(
            [](Object2D& c) { return Vec4{ [&] { return c.getBackgroundColor(); }, [&](auto v) { c.setBackgroundColor(v); } }; },
            [](Object2D& c, const Vec4& v) { c.setBackgroundColor(v); }
        ),

        // Field boolean enabled, Determines if this object is hoverable and clickable.
        "enabled", sol::property(&Object2D::getBorder, &Object2D::setBorder),

        // Field Event onHovered, Event called by Lime when this object is hovered.
        "onHovered", &Object2D::onHovered,
        // Field Event onPressed, Event called by Lime when this object is pressed.
        "onPressed", &Object2D::onPressed
    );

    // Parents this object to another 2D object.
    // Params any child
    // Returns void
    obj.set_function("parentTo", &Object2D::parentTo);

    // Returns true if this object is currently hovered.
    // Returns boolean
    obj.set_function("isHovered", &Object2D::isHovered);

    // Moves this object to the front in terms of z ordering. (Rendered last, overlaps all other objects)
    // Returns boolean
    obj.set_function("moveToFront", &Object2D::bringToFront);

    // Moves this object to the back in terms of z ordering. (Rendered first, all other objects then overlap)
    // Returns boolean
    obj.set_function("moveToBack", &Object2D::sendToBack);

    // In the case that the z ordering of the background for this object is above its children, this will update its z ordering to be correct.
    // Returns void
    obj.set_function("updateBackgroundZ", &Object2D::updateBackgroundZ);

    // End Interface
}