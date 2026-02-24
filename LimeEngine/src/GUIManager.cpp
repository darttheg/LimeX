#include "GUIManager.h"
#include "DebugConsole.h"
#include "Renderer.h"

#include <filesystem>
#include <unordered_map>
#include "Objects/Event.h"

#include "irrlicht.h"

static irr::gui::IGUIEnvironment* guienv = nullptr;
static DebugConsole* d = nullptr;
static Renderer* r = nullptr;

struct GUIManager::FCache {
	std::unordered_map<std::string, irr::gui::IGUIFont*> cache;
};

struct GUIManager::ButtonPair {
	std::shared_ptr<Event> onHover;
	std::shared_ptr<Event> onPressed;
};

struct GUIManager::ButtonCallbacks {
	std::unordered_map<irr::gui::IGUIElement*, ButtonPair> callbacks;
};

GUIManager::GUIManager(DebugConsole* de, Renderer* re) {
	d = de;
	r = re;

	fontCache = std::make_unique<FCache>();
}

GUIManager::~GUIManager() {
}

void GUIManager::SetGUIEnv(irr::gui::IGUIEnvironment* g) {
	if (!g) {
		d->Warn("GUI environment could not be created!");
		return;
	}

	guienv = g;
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

bool GUIManager::renderManually() {
	if (!guardRenderingCheck()) return false;
	if (!r->isManualRenderingOn()) {
		d->Warn("Manual rendering is not enabled. See Lime.setManualRendering.");
		return false;
	}

	Render();
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
		out += " is already embedded!";
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

void GUIManager::setDefaultFont(const std::string& name) {
	if (!guardRenderingCheck()) return;

	if (fontCache->cache.find(name) == fontCache->cache.end()) {
		std::string out = "Font ";
		out += name;
		out += " could not be made the default font: it is not embedded!";
		d->Warn(out);
		return;
	}

	irr::gui::IGUISkin* skin = guienv->getSkin();
	skin->setFont(fontCache->cache.find(name)->second);
}

bool GUIManager::isFontEmbedded(const std::string& name) {
	return fontCache->cache.find(name) != fontCache->cache.end();
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

void GUIManager::handleGUIEvent(irr::gui::IGUIElement* caller, irr::gui::IGUIElement* element, int eventType) {
	
}