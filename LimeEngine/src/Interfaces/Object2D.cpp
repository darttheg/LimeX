#include "Interfaces/Object2D.h"
#include "Objects/Event.h"
#include "Renderer.h"
#include "RenderHelper.h"
#include "Objects/Vec2.h"
#include "Objects/Vec4.h"

#include <sol/sol.hpp>
#include "irrlicht.h"
using namespace irr;
using namespace scene;

static Renderer* r = nullptr;
static RenderHelper* rh = nullptr;
static lua_State* l = nullptr;

struct ButtonPair {
    std::shared_ptr<Event> onHovered = nullptr;
    std::shared_ptr<Event> onPressed = nullptr;
};

Object2D::Object2D() {
    if (!rh->guardRenderingCheck("Cannot create 2D object prior to window creation.")) return;
}

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
    // if (bgBorder) bgBorder->setRelativePosition(irr::core::position2di(pos.getX(), pos.getY()));
    // if (button) button->setRelativePosition(irr::core::position2di(pos.getX(), pos.getY()));
    getNode()->updateAbsolutePosition();
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
    getNode()->setMaxSize(irr::core::dimension2du(size.getX(), size.getY()));
    getNode()->setRelativePosition(r);
    updateBorderDimensions();
    updateButtonDimensions();
}

bool Object2D::getBorder() const {
    return getNode() && hasBorder;
}

void Object2D::setBorder(bool enable) {
    if (!getNode()) return;


    if (auto* ebox = dynamic_cast<irr::gui::IGUIEditBox*>(getNode())) {
        ebox->setDrawBorder(enable);
        hasBorder = enable;
        return;
    }

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
    if (!getNode()) return false;

    if (!parent || *parent == nullptr) {
        auto* root = rh->getGUIRoot();
        if (!root) return false;

        root->addChild(getNode());
        // if (bgBorder) root->addChild(bgBorder);
        return true;
    }

    Object2D* p = *parent;
    if (!p->getNode()) return false;
    p->getNode()->addChild(getNode());
    // if (bgBorder) p->getNode()->addChild(bgBorder);
    // if (button) p->getNode()->addChild(button);
    return true;
}

bool Object2D::hasParent() {
    if (!getNode()) return false;
    return getNode()->getParent() != rh->getGUIRoot();
}

void Object2D::setHoverEvent(std::shared_ptr<Event> e) {
    onHovered = std::move(e);
}

void Object2D::setPressedEvent(std::shared_ptr<Event> e) {
    onPressed = std::move(e);
}

void Object2D::updateBorderDimensions() {
    if (!getNode() || !bgBorder) return;
    irr::core::recti ns = getNode()->getRelativePosition();
    bgBorder->setRelativePosition(irr::core::recti(0, 0, ns.getWidth(), ns.getHeight()));
}

void Object2D::updateButtonDimensions() {
    if (!getNode() || !button) return;
    irr::core::recti ns = getNode()->getRelativePosition();
    button->setRelativePosition(irr::core::recti(0, 0, ns.getWidth(), ns.getHeight()));
}

void Object2D::setBGBorder() {
    if (!getNode()) return;

    if ((hasBorder || hasBackground) && !bgBorder) {
        bgBorder = rh->createStaticText();
        bgBorder->setVisible(getNode()->isVisible());
        // getNode()->getParent()->addChild(bgBorder);
        // getNode()->getParent()->sendToBack(bgBorder);
        getNode()->addChild(bgBorder);
        bgBorder->setDrawBackground(true);
        bgBorder->setBackgroundColor(irr::video::SColor(backgroundColor.w, backgroundColor.x, backgroundColor.y, backgroundColor.z));
        updateBorderDimensions();
        // bgBorder->setRelativePosition(irr::core::position2di(getPosition().getX(), getPosition().getY()));
    } else if (!(hasBorder || hasBackground) && bgBorder) {
        bgBorder->drop();
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
    button = rh->createButton();
    if (!button) return;
    button->setEnabled(btnEnabled);
    button->setVisible(true);
    getNode()->addChild(button);
    updateButtonDimensions();

    r->addButtonPair(button, ButtonPair{onHovered, onPressed});

    // TODO: Make button loose and adjust position with viewport tl coordinates to fix collision.
}

void Object2D::removeButton() {
    if (!button) return;
    r->removeButtonPair(button);
    button->remove();
    button = nullptr;
}

void Object2D::checkButtonState() {
    if (!onHovered || !onPressed) return;
    int sz = onHovered->getSize() + onPressed->getSize();
    if (!button && sz > 0)
        createButton();
    else if (button && sz == 0)
        removeButton();
}

bool Object2D::isHovered() const {
    return r->isElementHovered(getNode());
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

int Object2D::getRefCount() const {
    return getNode() ? getNode()->getReferenceCount() : -1;
}

Vec2 Object2D::getAbsolutePosition() const {
    if (!getNode()) return Vec2();
    irr::core::recti pos = getNode()->getAbsolutePosition();
    return Vec2(pos.UpperLeftCorner.X, pos.UpperLeftCorner.Y);
}

sol::object Object2D::i_destroy() {
    onHovered->setOnLengthChanged(nullptr);
    onPressed->setOnLengthChanged(nullptr);
    onHovered->clear();
    onPressed->clear();

    removeButton();
    setBorder(false);
    // if (getNode()) getNode()->drop();
    destroy();
    return sol::make_object(l, sol::nil);
}

void Interface::Object2DBind::bind(lua_State* ls, Renderer* rend) {
    r = rend;
    rh = rend->GetRenderHelper();
    l = ls;
    sol::state_view view(ls);

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

        // Field boolean enabled, Determines if this object will process hooked input events, if any.
        "enabled", sol::property(&Object2D::getEnabled, &Object2D::setEnabled),

        // Field Vec4 backgroundColor, The RGBA background color of this object.
        "backgroundColor", sol::property(
            [](Object2D& c) { return Vec4{ [&] { return c.getBackgroundColor(); }, [&](auto v) { c.setBackgroundColor(v); } }; },
            [](Object2D& c, const Vec4& v) { c.setBackgroundColor(v); }
        ),

        // Field Event onHovered, Event called by Lime when this object is hovered.
        "onHovered", &Object2D::onHovered,
        // Field Event onPressed, Event called by Lime when this object is pressed.
        "onPressed", &Object2D::onPressed
    );

    // Parents this object to another 2D object.
    // Params any parent
    // Returns boolean
    obj.set_function("parentTo", &Object2D::parentTo);

    // Returns true if this object is parented to another 2D object.
    // Returns boolean
    obj.set_function("hasParent", &Object2D::hasParent);

    // Returns true if this object is currently hovered.
    // Returns boolean
    obj.set_function("isHovered", &Object2D::isHovered);

    // Moves this object to the front in terms of z ordering. (Rendered last, overlaps all other objects)
    // Returns boolean
    obj.set_function("moveToFront", &Object2D::bringToFront);

    // Moves this object to the back in terms of z ordering. (Rendered first, all other objects then overlap)
    // Returns boolean
    obj.set_function("moveToBack", &Object2D::sendToBack);

    // Returns the reference count for this object.
    // Returns number
    obj.set_function("getReferenceCount", &Object2D::getRefCount);

    // Returns the absolute position of this object.
    // Returns Vec2
    obj.set_function("getAbsolutePosition", &Object2D::getAbsolutePosition);

    // Destroys this object.
    // Returns nil
    obj.set_function("destroy", &Object2D::i_destroy);

    // End Interface
}