#define LIMEENGINE_EXPORT
#include "LimeEngine.h"
#include "Application.h"

extern "C" {

	LimeHandle Lime_Create() {
		Application* app = new Application();
		return reinterpret_cast<LimeHandle>(app);
	}

	void Lime_Run(LimeHandle handle, const void* data, size_t size) {
		Application* app = reinterpret_cast<Application*>(handle);
		if (!app) return;

		if (!app->Init(data, size)) return;

		app->Run();
	}

	void Lime_End(LimeHandle handle) {
		Application* app = reinterpret_cast<Application*>(handle);
		if (!app) return;

		app->EndApp();
		delete app;
	}
}