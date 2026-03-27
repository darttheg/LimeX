#include "Objects/Text3D.h"

#include <sol/sol.hpp>

#include "Objects/Vec2.h"
#include "RenderHelper.h"
#include "GUIManager.h"

#include "External/CGUIColoredText.h"
#include "External/CTextAnchorSceneNode.h"

static RenderHelper* rh = nullptr;
static GUIManager* g = nullptr;

Text3D::Text3D() {
	src = rh->createColoredText2D();
	wrap = rh->createText3DNode(src);
	if (!wrap || !src)
		return;

	src->setText(L"Text");
}

Text3D::Text3D(const std::string& tx) : Text3D() {
	if (!src) return;

	setText(tx);
}

void Text3D::destroy() {
	if (src) src->remove();
	if (wrap) wrap->remove();
	src, wrap = nullptr, nullptr;
}

void Text3D::setDebug(bool v) {
	if (v) {
		if (dVisual) dVisual->remove();
		dVisual = rh->createDebugNode(DEBUG3D_TYPE::TEXT);
	} else {
		if (dVisual) dVisual->remove();
	}
}

std::string Text3D::getText() {
	if (!src) return "";

	irr::core::stringw og = src->getText();
	std::string out(og.c_str(), og.c_str() + og.size());

	return out;
}

void Text3D::setText(const std::string& tx) {
	if (!src) return;

	std::wstring out(tx.begin(), tx.end());
	src->setText(out.c_str());
}

int Text3D::getOpacity() {
	return (int)(src->getOpacity() * 255.0f);
}

void Text3D::setOpacity(int o) {
	if (!src) return;

	src->setOpacity((float)o / 255.0f);
}

void Text3D::setAlignment(int all) {
	setAlignment(all, all);
}

void Text3D::setAlignment(int h, int v) {
	if (!src) return;
	src->setTextAlignment((irr::gui::EGUI_ALIGNMENT)h, (irr::gui::EGUI_ALIGNMENT)v);
}

void Text3D::setWordWrap(bool enable) {
	if (!src) return;
	src->setWordWrap(enable);
}

bool Text3D::setFont(const std::string& f) {
	if (!src) return false;

	irr::gui::IGUIFont* out = g->getGUIFont(f);
	if (!out) return false;

	src->setOverrideFont(out);
	return true;
}

Vec2 Text3D::getSize() const {
	if (!src) return Vec2();

	irr::core::recti r = src->getRelativePosition();
	return Vec2(r.LowerRightCorner.X - r.UpperLeftCorner.X, r.LowerRightCorner.Y - r.UpperLeftCorner.Y);
}

void Text3D::setSize(const Vec2& size) {
	if (!src) return;

	irr::core::recti r = src->getRelativePosition();
	r.LowerRightCorner = r.UpperLeftCorner + irr::core::position2di((int)size.getX(), (int)size.getY());
	src->setRelativePosition(r);
	src->updateAbsolutePosition();
}

irr::scene::ISceneNode* Text3D::getNode() const {
	return wrap;
}

void Object::Text3DBind::bind(lua_State* ls, RenderHelper* renh, GUIManager* gu) {
	rh = renh;
	g = gu;

	// Object Text3D, A basic 3D object to display text. This object is essentially a `Text2D` tied to a 3D scene node. Text objects support colors and basic styling. Use tags `<#HEX>` for color, `<s>` for strike, `<d>` for drop shadow, `<u>` for underline, `<b>` for bold, and `<r>` to reset styles. Example: `<#6ABE30>This is green! <b>Now, it's green and bold! <r>Now, it's back to normal.`
	// Inherits Object3D

	// Constructor
	// Constructor string text

	sol::state_view view(ls);
	sol::usertype<Text3D> obj = view.new_usertype<Text3D>(
		"Text3D",
		sol::constructors<Text3D(), Text3D(const std::string& t)>(),

		sol::base_classes, sol::bases<Object3D>(),
		sol::meta_function::type, [](const Text3D&) { return "Text3D"; },

		// Field string text, The text content of this object.
		"text", sol::property(&Text3D::getText, &Text3D::setText),

		// Field number opacity, The opacity of the text, from 0 to 255. For individual characters being not fully opaque, use color tags with an alpha value.
		"opacity", sol::property(&Text3D::getOpacity, &Text3D::setOpacity),

		// Field Vec2 size, The size of the 2D text box.
        "size", sol::property(
            [](Text3D& c) { return Vec2{ [&] { return c.getSize(); }, [&](auto v) { c.setSize(v); } }; },
            [](Text3D& c, const Vec2& v) { c.setSize(v); }
        )
	);

	obj[sol::meta_function::to_string] = [](const Text3D& v) {
		return "Text3D";
		};

	// Sets the font to use for this object. Fonts must first be loaded. See `Lime.GUI.loadXML` and `Lime.GUI.loadTTF`. 
	// Params string name
	// Returns boolean
	obj.set_function("setFont", &Text3D::setFont);

	// Enables word wrap.
	// Params boolean wrap
	// Returns void
	obj.set_function("setWordWrap", &Text3D::setWordWrap);

	// Sets the text's alignment within its bounding box.
	// Params Lime.Enum.TextAlign all
	// Params Lime.Enum.TextAlign x, Lime.Enum.TextAlign y
	// Returns void
	obj.set_function("setAlignment",
		sol::overload(
			sol::resolve<void(int)>(&Text3D::setAlignment),
			sol::resolve<void(int, int)>(&Text3D::setAlignment)
		));

	// End Object
}