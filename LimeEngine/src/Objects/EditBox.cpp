#include "Objects/EditBox.h"
#include "irrlicht.h"
#include <sol/sol.hpp>

#include "GUIManager.h"
#include "RenderHelper.h"
#include "Objects/Vec2.h"

static DebugConsole* d = nullptr;
static RenderHelper* rh = nullptr;
static GUIManager* g = nullptr;

EditBox::EditBox() {
	ebox = rh->createEditBox();
	if (!ebox) return;

	ebox->setText(L"");

	Object2D::createEvents();
}

EditBox::EditBox(const std::string& tx) : EditBox() {
	if (!ebox) return;

	setText(tx);
}

EditBox::EditBox(const Vec2& pos, const Vec2& sz) : EditBox() {
	if (!ebox) return;

	Object2D::setPosition(pos);
	Object2D::setSize(sz);
}

void EditBox::setAlignment(int all) {
	setAlignment(all, all);
}

void EditBox::setAlignment(int h, int v) {
	if (!ebox) return;
	ebox->setTextAlignment((irr::gui::EGUI_ALIGNMENT)h, (irr::gui::EGUI_ALIGNMENT)v);
}

void EditBox::destroy() {
	if (ebox) ebox->remove();
	ebox = nullptr;
	bg = true;
	wraps = false;
}

std::string EditBox::getText() {
	if (!ebox) return "";

	irr::core::stringw og = ebox->getText();
	std::string out(og.c_str(), og.c_str() + og.size());

	return out;
}

void EditBox::setText(const std::string& tx) {
	if (!ebox) return;

	std::wstring out(tx.begin(), tx.end());
	ebox->setText(out.c_str());
}

bool EditBox::setFont(const std::string& f) {
	if (!ebox) return false;

	irr::gui::IGUIFont* out = g->getGUIFont(f);
	if (!out) return false;

	ebox->setOverrideFont(out);
	return true;
}

bool EditBox::getIsPassword() {
	return ebox ? ebox->isPasswordBox() : false;
}

void EditBox::setIsPassword(bool enable) {
	if (ebox) ebox->setPasswordBox(enable);
}

bool EditBox::getWordWrap() {
	return ebox ? wraps : false;
}

void EditBox::setWordWrap(bool enable) {
	if (!ebox) return;
	ebox->setWordWrap(enable);
	wraps = enable;
}

bool EditBox::getIsEnabled() {
	return ebox ? ebox->isEnabled() : false;
}

void EditBox::setIsEnabled(bool enable) {
	if (ebox) ebox->setEnabled(enable);
}

bool EditBox::getMultiLine() {
	return ebox ? ebox->isMultiLineEnabled() : false;
}

void EditBox::setMultiLine(bool enable) {
	if (ebox) ebox->setMultiLine(enable);
}

bool EditBox::getAutoScroll() {
	return ebox ? ebox->isAutoScrollEnabled() : false;
}

void EditBox::setAutoScroll(bool enable) {
	if (ebox) ebox->setAutoScroll(enable);
}

bool EditBox::getFocused() {
	return ebox ? g->isElementFocused(ebox) : false;
}

void EditBox::setFocused(bool v) {
	if (!ebox) return;
	g->setElementFocused(ebox, v);
}

int EditBox::getMaxCharCount() const {
	return ebox ? ebox->getMax() : 0;
}

void EditBox::setMaxCharCount(int v) {
	if (!ebox) return;
	if (v < 0) v = 0;
	ebox->setMax(v);
}

int EditBox::getPos() {
	return ebox ? ebox->getCursorPosition() : 0;
}

void EditBox::setPos(int v) {
	if (!ebox) return;
	ebox->setCursorPosition(v);
}

bool EditBox::getIsDrawingBackground() {
	return ebox ? bg : false;
}

void EditBox::setIsDrawingBackground(bool v) {
	if (!ebox) return;
	ebox->setDrawBackground(v);
	bg = v;
}

irr::gui::IGUIElement* EditBox::getNode() const {
	return ebox;
}

void Object::EditBoxBind::bind(lua_State* ls, RenderHelper* renh, GUIManager* gu) {
	rh = renh;
	g = gu;

	// Object EditBox, A basic 2D object that allows user input. `EditBox` objects can be used for getting user input and can even be made to look like passwords.
	// Inherits Object2D

	// Constructor
	// Constructor string text
	// Constructor Vec2 pos, Vec2 size

	sol::state_view view(ls);
	sol::usertype<EditBox> obj = view.new_usertype<EditBox>(
		"EditBox",
		sol::constructors<EditBox(), EditBox(const std::string & t), EditBox(const Vec2 & p, const Vec2 & s)>(),

		sol::base_classes, sol::bases<Object2D>(),
		sol::meta_function::type, [](const EditBox&) { return "EditBox"; },

		// Field string text, The text content of this object.
		"text", sol::property(&EditBox::getText, &EditBox::setText),

		// Field boolean password, Determines if the text content of this `EditBox` is obfuscated.
		"password", sol::property(&EditBox::getIsPassword, &EditBox::setIsPassword),

		// Field boolean enabled, Determines if this `EditBox` is enabled.
		"enabled", sol::property(&EditBox::getEnabled, &EditBox::setEnabled),

		// Field number maxChars, The maximum amount of characters allowed.
		"maxChars", sol::property(&EditBox::getMaxCharCount, &EditBox::setMaxCharCount),

		// Field number cursorPosition, The position of the cursor in this `EditBox`.
		"cursorPosition", sol::property(&EditBox::getPos, &EditBox::setPos),

		// Field boolean multiLine, Determines if this `EditBox` supports multiple lines.
		"multiLine", sol::property(&EditBox::getMultiLine, &EditBox::setMultiLine),

		// Field boolean background, Determines if this `EditBox` has a visible background.
		"background", sol::property(&EditBox::getIsDrawingBackground, &EditBox::setIsDrawingBackground),

		// Field boolean autoscroll, Determines if this `EditBox` will autoscroll.
		"autoscroll", sol::property(&EditBox::getAutoScroll, &EditBox::setAutoScroll),

		// Field boolean focused, Determines if this `EditBox` is focused.
		"focused", sol::property(&EditBox::getFocused, &EditBox::setFocused),

		// Field boolean wordWrap, Determines if the text wraps when touching the border of its text box.
		"wordWrap", sol::property(&EditBox::getWordWrap, &EditBox::setWordWrap)
	);

	obj[sol::meta_function::to_string] = [](const EditBox& v) {
		return "EditBox";
		};

	// Sets the font to use for this object. Fonts must first be loaded. See `Lime.GUI.loadXML` and `Lime.GUI.loadTTF`. 
	// Params string name
	// Returns boolean
	obj.set_function("setFont", &EditBox::setFont);

	// Sets the text's alignment within its bounding box.
	// Params Lime.Enum.TextAlign all
	// Params Lime.Enum.TextAlign x, Lime.Enum.TextAlign y
	// Returns void
	obj.set_function("setAlignment",
		sol::overload(
			sol::resolve<void(int)>(&EditBox::setAlignment),
			sol::resolve<void(int, int)>(&EditBox::setAlignment)
		));

	// End Object
}