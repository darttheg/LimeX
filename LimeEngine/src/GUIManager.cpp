#include "GUIManager.h"
#include "DebugConsole.h"
#include "Renderer.h"

#include <filesystem>
#include <unordered_map>
#include "Objects/Event.h"
#include "Interfaces/Object2D.h"

// #include "irrlicht.h"
#include "External/FontLoader.h"

static irr::gui::IGUIEnvironment* guienv = nullptr;
static DebugConsole* d = nullptr;
static Renderer* r = nullptr;
static irr::IrrlichtDevice* device = nullptr;

struct GUIManager::FCache {
	std::unordered_map<std::string, irr::gui::IGUIFont*> cache;
};

struct GUIManager::ButtonPair {
	std::shared_ptr<Event> onHovered;
	std::shared_ptr<Event> onPressed;
};

GUIManager::GUIManager(DebugConsole* de, Renderer* re) {
	d = de;
	r = re;

	fontCache = std::make_unique<FCache>();
}

GUIManager::~GUIManager() {
}

void GUIManager::SetGUIEnv(irr::gui::IGUIEnvironment* g, irr::IrrlichtDevice* downer) {
	if (!g) {
		d->Warn("GUI environment could not be created!");
		return;
	}

	guienv = g;
	device = downer;
	setQuality(0);
}

bool GUIManager::Render() {
	if (!guardRenderingCheck()) return false;
	guienv->drawAll();

	return true;
}

bool GUIManager::guardRenderingCheck() {
	if (!guienv) {
		d->Warn("The GUI environment cannot be modified until the Lime window has been created!");
		return false;
	}
	return true;
}

// ---

std::string GUIManager::embedFont(const std::string& path) {
	if (!guardRenderingCheck()) return "";

	std::filesystem::path p(path);
	if (p.extension() != ".xml") {
		d->Warn("Font path must lead to a .xml file!");
		return "";
	}

	std::string fontName = p.stem().string();

	if (fontCache->cache.find(fontName) != fontCache->cache.end()) {
		std::string out = "Font ";
		out += fontName;
		out += " is already loaded!";
		d->Warn(out);
		return fontName;
	}

	gui::IGUIFont* font = guienv->getFont(path.c_str());
	if (!font) {
		std::string out = "Font ";
		out += fontName;
		out += " could not be loaded!";
		d->Warn(out);
		return "";
	}

	fontCache->cache[fontName] = font;
	return fontName;
}

std::string GUIManager::embedTTF(const std::string& ttfPath, int size, bool aa) {
	if (size <= 0) {
		d->Warn("Failed to load TTF font " + ttfPath + ": Font size " + std::to_string(size) + " is too small.");
		return "";
	}

	std::string name;
	irr::gui::IGUIFont* out = FontLoader::loadTTFAuto(device, ttfPath.c_str(), size, &name, aa);
	if (!out) {
		d->Warn("Failed to load TTF font " + ttfPath);
		return "";
	}

	fontCache->cache[name] = out;
	return name;
}

std::string GUIManager::embedTTF(const std::string& ttfPath, int size, const std::string& name, bool aa) {
	if (size <= 0) {
		d->Warn("Failed to load TTF font " + ttfPath + ": Font size " + std::to_string(size) + " is too small.");
		return "";
	}

	irr::gui::IGUIFont* out = FontLoader::loadTTF(device, ttfPath.c_str(), size, name.c_str(), aa);
	if (!out) {
		d->Warn("Failed to load TTF font " + ttfPath);
		return "";
	}

	fontCache->cache[name] = out;
	return name;
}

void GUIManager::setDefaultFont(const std::string& name) {
	if (!guardRenderingCheck()) return;

	if (fontCache->cache.find(name) == fontCache->cache.end()) {
		std::string out = "Font ";
		out += name;
		out += " could not be made the default font: it is not loaded!";
		d->Warn(out);
		return;
	}

	irr::gui::IGUISkin* skin = guienv->getSkin();
	skin->setFont(fontCache->cache.find(name)->second);
}

bool GUIManager::isFontEmbedded(const std::string& name) {
	return fontCache->cache.find(name) != fontCache->cache.end();
}

bool GUIManager::isElementFocused(irr::gui::IGUIElement* e) {
	if (!guardRenderingCheck()) return false;
	auto* focused = guienv->getFocus();
	return focused && focused == e;
}

void GUIManager::setElementFocused(irr::gui::IGUIElement* e, bool v) {
	if (!guardRenderingCheck()) return;
	if (v) guienv->setFocus(e); else guienv->setFocus(nullptr);
}

void GUIManager::setQuality(int q) {
	if (!guardRenderingCheck()) return;

	r->setGUIQuality(q);
}

irr::gui::IGUIFont* GUIManager::getGUIFont(const std::string& name) {
	if (!guardRenderingCheck()) return nullptr;

	auto it = fontCache->cache.find(name);
	if (it != fontCache->cache.end()) {
		return it->second;
	}

	return nullptr;
}

void GUIManager::addButtonPair(const Object2D& o) {
	auto it = buttonCallbacks.find(o.getButton());
	if (it != buttonCallbacks.end()) return;

	ButtonPair pb{ o.onHovered, o.onPressed };
	buttonCallbacks.emplace(o.getButton(), pb);
}

void GUIManager::removeButtonPair(const Object2D& o) {
	auto it = buttonCallbacks.find(o.getButton());
	if (it != buttonCallbacks.end())
		buttonCallbacks.erase(o.getButton());
}

void GUIManager::handleGUIEvent(irr::gui::IGUIElement* caller, irr::gui::IGUIElement* element, int eventType) {
	auto it = buttonCallbacks.find(caller);
	if (it != buttonCallbacks.end()) {
		irr::gui::EGUI_EVENT_TYPE type = static_cast<irr::gui::EGUI_EVENT_TYPE>(eventType);

		switch (type) {
		case irr::gui::EGUI_EVENT_TYPE::EGET_BUTTON_CLICKED:
			if (!r->runEventFromGUI(it->second.onPressed, [&](const std::string& msg) { d->PostError(msg); }))
				d->Warn("Could not run onPressed Event: Event is not valid!");
			break;
		case irr::gui::EGUI_EVENT_TYPE::EGET_ELEMENT_HOVERED:
			if (!r->runEventFromGUI(it->second.onHovered, [&](const std::string& msg) { d->PostError(msg); }))
				d->Warn("Could not run onHovered Event: Event is not valid!");
			break;
		}
	}
}