#define LIMEENGINE_EXPORT
#include "LimeEngine.h"
#include "Application.h"

#include "Window.h"

extern "C" {

	LimeHandle Lime_Create() {
		Application* app = new Application();
		return reinterpret_cast<LimeHandle>(app);
	}

	int Lime_Run(LimeHandle handle, const void* data, size_t size) {
		Application* app = reinterpret_cast<Application*>(handle);
		if (!app) return 1;

		if (!app->Init(data, size)) return 1;

		return app->Run();
	}

	void Lime_End(LimeHandle handle) {
		Application* app = reinterpret_cast<Application*>(handle);
		if (!app) return;

		app->EndApp();
		delete app;
	}
}