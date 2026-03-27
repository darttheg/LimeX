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
	}
	class IrrlichtDevice;
}
using namespace irr;

// This class lays under the Renderer, as GUI parts are elements of Irrlicht.
class GUIManager {
public:
	GUIManager(DebugConsole* de, Renderer* re);
	~GUIManager();

	void SetGUIEnv(irr::gui::IGUIEnvironment* g, irr::IrrlichtDevice* downer);
	bool Render();

	bool guardRenderingCheck();
	std::string embedFont(const std::string& path);
	std::string embedTTF(const std::string& ttfPath, int size, bool aa);
	std::string embedTTF(const std::string& ttfPath, int size, const std::string& name, bool aa);
	void setDefaultFont(const std::string& name);
	bool isFontEmbedded(const std::string& name);
	void setQuality(int q);
	irr::gui::IGUIFont* getGUIFont(const std::string& name);
	void addButtonPair(const Object2D& o);
	void removeButtonPair(const Object2D& o);
	void handleGUIEvent(irr::gui::IGUIElement* caller, irr::gui::IGUIElement* element, int eventType);
private:
	struct FCache;
	std::unique_ptr<FCache> fontCache;

	struct ButtonPair;
	std::unordered_map<irr::gui::IGUIElement*, ButtonPair> buttonCallbacks;
};