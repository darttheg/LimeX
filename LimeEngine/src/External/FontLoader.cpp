#include "External/FontLoader.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cstdio>
#include <string>

namespace FontLoader {
    static std::string autoKey(const char* ttfPath, int size) {
        std::string name = ttfPath;
        size_t slash = name.find_last_of("/\\");
        if (slash != std::string::npos) name = name.substr(slash + 1);
        size_t dot = name.find_last_of('.');
        if (dot != std::string::npos) name = name.substr(0, dot);
        return name + "_" + std::to_string(size);
    }

    irr::gui::IGUIFont* loadTTF(irr::IrrlichtDevice* device, const char* ttfPath, int size, const char* fontName, bool aa)
    {
        FT_Library ft;
        FT_Face    face;
        if (FT_Init_FreeType(&ft)) return nullptr;
        if (FT_New_Face(ft, ttfPath, 0, &face)) {
            FT_Done_FreeType(ft);
            return nullptr;
        }
        FT_Set_Pixel_Sizes(face, 0, size);

        const irr::u32 atlasW = 512;
        const irr::u32 atlasH = 512;
        FT_Int32 loadFlags = FT_LOAD_RENDER | (aa ? 0 : FT_LOAD_TARGET_MONO);

        struct GlyphArea {
            irr::core::rect<irr::s32> rect;
            irr::s32 underhang;
            irr::s32 overhang;
        };

        irr::core::map<wchar_t, GlyphArea> charMap;

        irr::s32 penX = 0, penY = 0, rowH = 0;

        for (FT_ULong c = 32; c < 127; c++) {
            if (FT_Load_Char(face, c, loadFlags)) continue;
            FT_GlyphSlot g = face->glyph;
            irr::u32 w = g->bitmap.width;
            irr::u32 h = g->bitmap.rows;
            if (w == 0 || h == 0) continue;

            if (penX + (irr::s32)w > (irr::s32)atlasW) {
                penX = 0;
                penY += rowH + 1;
                rowH = 0;
            }

            irr::u32 cellH = (face->size->metrics.ascender - face->size->metrics.descender) >> 6;
            GlyphArea ga;
            ga.rect = irr::core::rect<irr::s32>(penX, penY, penX + w, penY + cellH);
            ga.underhang = g->bitmap_left;
            ga.overhang = (g->advance.x >> 6) - g->bitmap_left - (irr::s32)w;
            charMap.insert(c, ga);

            penX += (irr::s32)w + 1;
            if ((irr::s32)h + 1 > rowH) rowH = (irr::s32)h + 1;
        }

        irr::u32 finalH = (irr::u32)(penY + rowH);
        irr::u32 texH = 1;
        while (texH < finalH) texH <<= 1;
        if (texH > atlasH) texH = atlasH;

        irr::video::IVideoDriver* driver = device->getVideoDriver();

        irr::video::IImage* img = driver->createImage(irr::video::ECF_A8R8G8B8, irr::core::dimension2du(atlasW, texH));
        img->fill(irr::video::SColor(0, 0, 0, 0));

        for (FT_ULong c = 32; c < 127; c++) {
            if (!charMap.find(c)) continue;
            if (FT_Load_Char(face, c, loadFlags)) continue;

            FT_Bitmap& bmp = face->glyph->bitmap;
            irr::core::map<wchar_t, GlyphArea>::Node* node = charMap.find(c);
            if (!node) continue;
            GlyphArea& ga = node->getValue();

            irr::u32 yDest = ga.rect.UpperLeftCorner.Y + (face->size->metrics.ascender >> 6) - face->glyph->bitmap_top;
            irr::u32 xDest = ga.rect.UpperLeftCorner.X;

            for (irr::u32 row = 0; row < bmp.rows; row++)
                for (irr::u32 col = 0; col < bmp.width; col++) {
                    irr::u8 v = aa ? bmp.buffer[row * bmp.pitch + col] : ((bmp.buffer[row * bmp.pitch + col / 8] & (0x80 >> (col % 8))) ? 255 : 0);
                    img->setPixel(
                        xDest + col,
                        yDest + row,
                        irr::video::SColor(v, 255, 255, 255));
                }
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        irr::core::stringc imgPath = fontName; imgPath += ".png";
        irr::core::stringc xmlPath = fontName; xmlPath += ".xml";

        driver->writeImageToFile(img, imgPath.c_str());
        img->drop();

        irr::io::IXMLWriter* writer = device->getFileSystem()->createXMLWriter(xmlPath.c_str());
        writer->writeXMLHeader();
        writer->writeLineBreak();
        writer->writeElement(L"font", false, L"type", L"bitmap");
        writer->writeLineBreak();
        writer->writeLineBreak();

        writer->writeElement(L"Texture", true, L"index", L"0", L"filename", irr::core::stringw(imgPath.c_str()).c_str(), L"hasAlpha", L"true");
        writer->writeLineBreak();
        writer->writeLineBreak();

        for (irr::core::map<wchar_t, GlyphArea>::Iterator it = charMap.getIterator(); !it.atEnd(); it++) {
            wchar_t ch[2] = { (*it).getKey(), L'\0' };
            GlyphArea& ga = (*it).getValue();

            irr::core::stringw area;
            area = irr::core::stringw(ga.rect.UpperLeftCorner.X);  area += L", ";
            area += ga.rect.UpperLeftCorner.Y;                       area += L", ";
            area += ga.rect.LowerRightCorner.X;                      area += L", ";
            area += ga.rect.LowerRightCorner.Y;

            irr::core::array<irr::core::stringw> names, values;
            names.push_back(L"c"); values.push_back(irr::core::stringw(ch));
            names.push_back(L"r"); values.push_back(area);
            if (ga.underhang != 0) { names.push_back(L"u"); values.push_back(irr::core::stringw(ga.underhang)); }
            if (ga.overhang != 0) { names.push_back(L"o"); values.push_back(irr::core::stringw(ga.overhang)); }

            writer->writeElement(L"c", true, names, values);
            writer->writeLineBreak();
        }

        writer->writeClosingTag(L"font");
        writer->drop();

        irr::gui::IGUIFont* font = device->getGUIEnvironment()->getFont(xmlPath.c_str());

        remove(imgPath.c_str());
        remove(xmlPath.c_str());

        return font;
    }

    irr::gui::IGUIFont* loadTTFAuto(irr::IrrlichtDevice* device, const char* ttfPath, int size, std::string* outName, bool aa) {
        std::string key = autoKey(ttfPath, size);
        if (outName) *outName = key;
        return loadTTF(device, ttfPath, size, key.c_str(), aa);
    }
}