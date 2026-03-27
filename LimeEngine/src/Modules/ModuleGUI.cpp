#include "Modules/ModuleGUI.h"

#include "DebugConsole.h"
#include "Application.h"
#include "Renderer.h"
#include "GUIManager.h"

#include "Objects/Event.h"
#include "Objects/Texture.h"

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

	// Loads a bitmap font. Returns the name of this font, cut from `path`. (NOTE: `path` must be the path to a .xml file. The .xml files must be paired by an image file.)
	// Params string path
	// Returns string
	module.set_function("loadXML", &Module::GUI::Bind::embedFont);

	// Sets the default font for new GUI elements to font `name`.
	// Params string name
	// Returns void
	module.set_function("setDefaultFont", &Module::GUI::Bind::setDefaultFont);

	// Returns true if the font `name` is loaded.
	// Params string name
	// Returns boolean
	module.set_function("isFontLoaded", &Module::GUI::Bind::isFontEmbedded);

	// Sets the quality of all GUI elements using `Lime.Enum.Quality` presets, where Low is unfiltered and High is smooth.
	// Params Lime.Enum.Quality quality
	// Returns void
	module.set_function("setQuality", &Module::GUI::Bind::setQuality);

	// Loads a Truetype font. Provide `name` to set the name manually, otherwise Lime will register the font as fontname_size. Returns the output font name.
	// Params string path, number fontSize, boolean? aa
	// Params string path, number fontSize, string name, boolean? aa
	// Returns string
	module.set_function("loadTTF",
		sol::overload(
			[](const std::string& path, int size) { return Module::GUI::Bind::embedTTF(path, size, true);  },
			[](const std::string& path, int size, bool aa) { return Module::GUI::Bind::embedTTF(path, size, aa);  },
			[](const std::string& path, int size, const std::string& name) { return Module::GUI::Bind::embedTTF(path, size, name, true);  },
			[](const std::string& path, int size, const std::string& name, bool aa) { return Module::GUI::Bind::embedTTF(path, size, name, aa);  }
		));

	// End Module
}

// Functions

std::string Module::GUI::Bind::embedFont(const std::string& path) {
	return g->embedFont(path);
}

void Module::GUI::Bind::setDefaultFont(const std::string& name) {
	g->setDefaultFont(name);
}

bool Module::GUI::Bind::isFontEmbedded(const std::string& name) {
	return g->isFontEmbedded(name);
}

void Module::GUI::Bind::setQuality(int q) {
	g->setQuality(q);
}

std::string Module::GUI::Bind::embedTTF(const std::string& ttfPath, int size, bool aa) {
	return g->embedTTF(ttfPath, size, aa);
}

std::string Module::GUI::Bind::embedTTF(const std::string& ttfPath, int size, const std::string& name, bool aa) {
	return g->embedTTF(ttfPath, size, name, aa);
}