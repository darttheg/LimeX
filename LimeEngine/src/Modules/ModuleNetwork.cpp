#include "Modules/ModuleNetwork.h"

#include "Application.h"
#include "NetworkManager.h"
#include "Objects/Event.h"

#include <sol/sol.hpp>

static Application* a;
static NetworkManager* n;

void Module::Network::bind(Application* app) {
	a = app;
	n = app->GetNetworkManager();
	sol::state& lua = app->GetLuaState();

	// Module Lime.Network
	sol::table module = lua["Lime"]["Network"].get_or_create<sol::table>();

	

	// End Module
}

// Functions