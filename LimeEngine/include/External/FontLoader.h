#pragma once
#include <string>
#include <irrlicht.h>

namespace FontLoader {
    irr::gui::IGUIFont* loadTTF(irr::IrrlichtDevice* device, const char* ttfPath, int size, const char* fontName);
    irr::gui::IGUIFont* loadTTFAuto(irr::IrrlichtDevice* device, const char* ttfPath, int size, std::string* outName = nullptr);
}