#pragma once
#include <array>
#include <string>
#include <memory>
#include <unordered_map>

class DebugConsole;
class Renderer;
class Object2D;

namespace irr {
	namespace gui {
		class IGUIEnvironment;
		class IGUIFont;
		class IGUIElement;
		class IGUIButton;
	}
	class IrrlichtDevice;
}
using namespace irr;

struct ButtonPair;

// This class lays under the Renderer, as GUI parts are elements of Irrlicht.
class GUIManager {
public:
	GUIManager(Renderer* re, DebugConsole* de);
	~GUIManager();

	void SetGUIEnv(irr::gui::IGUIEnvironment* g, irr::IrrlichtDevice* downer);
	bool Render();

	bool guardRenderingCheck();
	std::string embedFont(const std::string& path);
	std::string embedTTF(const std::string& ttfPath, int size, bool aa);
	std::string embedTTF(const std::string& ttfPath, int size, const std::string& name, bool aa);
	void setDefaultFont(const std::string& name);
	bool isFontEmbedded(const std::string& name);
	bool isAnyElementHovered();
	bool isElementFocused(irr::gui::IGUIElement* e);
	void setElementFocused(irr::gui::IGUIElement* e, bool v);
	void setQuality(int q);
	irr::gui::IGUIFont* getGUIFont(const std::string& name);
	void addButtonPair(irr::gui::IGUIButton* button, ButtonPair);
	void removeButtonPair(irr::gui::IGUIButton* element);
	void handleGUIEvent(irr::gui::IGUIElement* caller, irr::gui::IGUIElement* element, int eventType);
private:
	struct FCache;
	std::unique_ptr<FCache> fontCache;

	std::unordered_map<irr::gui::IGUIElement*, ButtonPair> buttonCallbacks;
};