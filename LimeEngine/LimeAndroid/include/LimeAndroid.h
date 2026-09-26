#pragma once

#include <string>
#include <vector>

struct android_app;
namespace irr { class IrrlichtDevice; }

namespace LimeAndroid {
	// from AndroidMain
	android_app* getApp();
	const unsigned char* getResourcesZip(unsigned long* size);
	const char* getDataDir(); // also the working directory
	void showSoftKeyboard(irr::IrrlichtDevice* device, bool show);

	struct KeyboardInput {
		int keyCode = 0; // 0 for text
		std::u16string text;
	};
	void takeKeyboardInput(std::vector<KeyboardInput>& out);

	// CIrrDeviceAndroid funcs
	bool getSurfaceSize(irr::IrrlichtDevice* device, int* width, int* height);
	void setSwapInterval(irr::IrrlichtDevice* device, int interval);
	void setPauseListener(void (*listener)(bool paused));
}
