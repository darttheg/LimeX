#include "Objects/Text2D.h"
#include "irrlicht.h"

#include "External/CGUIColoredText.h"
#include "Application.h"
#include "DebugConsole.h"
#include "GUIManager.h"
#include "Renderer.h"
#include "Objects/Vec2.h"

static DebugConsole* d = nullptr;
static Renderer* r = nullptr;
static GUIManager* g = nullptr;

Text2D::Text2D() {
	text = r->createColoredText2D();
	if (!text) {
		d->Warn("Could not create Text2D");
		return;
	}

	text->setText(L"Text");

	Object2D::createEvents();
}

Text2D::Text2D(const std::string& tx) : Text2D() {
	if (!text) return;

	setText(tx);
}

Text2D::Text2D(const Vec2& pos, const Vec2& sz) : Text2D() {
	if (!text) return;

	Object2D::setPosition(pos);
	Object2D::setSize(sz);
}

void Text2D::destroy() {
	if (text) text->remove();
	text = nullptr;
}

std::string Text2D::getText() {
	if (!text) return "";

	irr::core::stringw og = text->getText();
	std::string out(og.c_str(), og.c_str() + og.size());

	return out;
}

void Text2D::setText(const std::string& tx) {
	if (!text) return;

	std::wstring out(tx.begin(), tx.end());
	text->setText(out.c_str());
}

int Text2D::getOpacity() {
	return (int)(text->getOpacity() * 255.0f);
}

void Text2D::setOpacity(int o) {
	if (!text) return;

	text->setOpacity((float)o / 255.0f);
}

void Text2D::setAlignment(int all) {
	setAlignment(all, all);
}

void Text2D::setAlignment(int h, int v) {
	if (!text) return;
	text->setTextAlignment((irr::gui::EGUI_ALIGNMENT)h, (irr::gui::EGUI_ALIGNMENT)v);
}

void Text2D::setWordWrap(bool enable) {
	if (!text) return;
	text->setWordWrap(enable);
}

bool Text2D::setFont(const std::string& f) {
	if (!text) return false;

	irr::gui::IGUIFont* out = g->getGUIFont(f);
	if (!out) return false;

	text->setOverrideFont(out);
	return true;
}

irr::gui::IGUIElement* Text2D::getNode() const {
	return text;
}

void Object::Text2DBind::bind(Application* a) {
	r = a->GetRenderer();
	d = a->GetDebugConsole();
	g = r->getGUIManager();

	// Object Text2D, A basic 2D object to display text. Text objects support colors and basic styling. Use tags `<#HEX>` for color, `<s>` for strike, `<d>` for drop shadow, `<u>` for underline, `<b>` for bold, and `<r>` to reset styles. Example: `<#6ABE30>This is green! <b>Now, it's green and bold! <r>Now, it's back to normal.`
	// Inherits Object2D

	// Constructor
	// Constructor string text
	// Constructor Vec2 pos, Vec2 size

	sol::state_view view(a->GetLuaState());
	sol::usertype<Text2D> obj = view.new_usertype<Text2D>(
		"Text2D",
		sol::constructors<Text2D(), Text2D(const std::string& t), Text2D(const Vec2& p, const Vec2& s)>(),

		sol::base_classes, sol::bases<Object2D>(),
		sol::meta_function::type, [](const Text2D&) { return "Text2D"; },

		// Field string text, The text content of this object.
		"text", sol::property(&Text2D::getText, &Text2D::setText),

		// Field number opacity, The opacity of the text, from 0 to 255. For individual characters being not fully opaque, use color tags with an alpha value.
		"opacity", sol::property(&Text2D::getOpacity, &Text2D::setOpacity)
	);

	obj[sol::meta_function::to_string] = [](const Text2D& v) {
		return "Text2D";
		};

	// Sets the font to use for this object. Fonts must first be embedded. See `Lime.GUI.embedFont`. 
	// Params string name
	// Returns boolean
	obj.set_function("setFont", &Text2D::setFont);

	// Enables word wrap.
	// Params boolean wrap
	// Returns void
	obj.set_function("setWordWrap", &Text2D::setWordWrap);

	// Sets the text's alignment within its bounding box.
	// Params Lime.Enum.TextAlign all
	// Params Lime.Enum.TextAlign x, Lime.Enum.TextAlign y
	// Returns void
	obj.set_function("setAlignment",
		sol::overload(
			sol::resolve<void(int)>(&Text2D::setAlignment),
			sol::resolve<void(int, int)>(&Text2D::setAlignment)
		));

	// End Object
}