#pragma once
#include <array>
#include <string>
#include <memory>

class DebugConsole;
class Renderer;

namespace irr {
	namespace gui {
		class IGUIEnvironment;
	}

	class IrrlichtDevice;
}
using namespace irr;

// This class lays under the Renderer, as GUI parts are elements of Irrlicht.
class GUIManager {
public:
	GUIManager(DebugConsole* de, Renderer* re);
	~GUIManager();

	void SetGUIEnv(irr::gui::IGUIEnvironment* g);
	bool Render();

	bool guardRenderingCheck();
	std::string embedFont(const std::string& path);
	void setDefaultFont(const std::string& name);
	bool isFontEmbeded(const std::string& name);
	void setQuality(int q);

private:
	struct FCache;
	std::unique_ptr<FCache> fontCache;
};