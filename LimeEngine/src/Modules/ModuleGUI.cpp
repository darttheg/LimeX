#include "Modules/ModuleGUI.h"

#include "DebugConsole.h"
#include "Application.h"
#include "Renderer.h"
#include "GUIManager.h"

#include "Objects/Event.h"
#include "Objects/Image.h"

#include <sol/sol.hpp>

static DebugConsole* d;
static Application* a;
static Renderer* r;
static GUIManager* g;

void Module::GUI::bind(Application* app) {
	a = app;
	d = app->GetDebugConsole();
	r = app->GetRenderer();
	g = r->getGUIManager();
	sol::state& lua = app->GetLuaState();

	// Module Lime.GUI
	sol::table module = lua["Lime"]["GUI"].get_or_create<sol::table>();

	// Renders all GUI elements to the screen. Returns true on success. (NOTE: Manual rendering must be on, otherwise this function has no effect. See `Lime.setManualRendering`.)
	// Returns boolean
	module.set_function("render", &Module::GUI::Bind::render);

	// Embeds a bitmap font from path `path`. Returns the name of this font, cut from `path`. (NOTE: `path` must be the path to a .xml file. The .xml files must be paired by an image file.)
	// Params string path
	// Returns string
	module.set_function("embedFont", &Module::GUI::Bind::embedFont);

	// Sets the default font for new GUI elements to font `name`.
	// Params string name
	// Returns void
	module.set_function("setDefaultFont", &Module::GUI::Bind::setDefaultFont);

	// Returns true if the font `name` is embeded.
	// Params string name
	// Returns boolean
	module.set_function("isFontEmbeded", &Module::GUI::Bind::isFontEmbeded);

	// Sets the quality of all GUI elements using `Lime.Enum.Quality` presets, where Low is unfiltered and High is smooth.
	// Params Lime.Enum.Quality quality
	// Returns void
	module.set_function("setQuality", &Module::GUI::Bind::setQuality);

	// End Module
}

// Functions

bool Module::GUI::Bind::render() {
	return g->Render();
}

std::string Module::GUI::Bind::embedFont(const std::string& path) {
	return g->embedFont(path);
}

void Module::GUI::Bind::setDefaultFont(const std::string& name) {
	g->setDefaultFont(name);
}

bool Module::GUI::Bind::isFontEmbeded(const std::string& name) {
	return g->isFontEmbeded(name);
}

void Module::GUI::Bind::setQuality(int q) {
	g->setQuality(q);
}