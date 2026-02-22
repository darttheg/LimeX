#pragma once
#include <irrlicht.h>
#include <vector>
using namespace irr;

namespace irr::gui {
    class CGUIColoredText : public gui::IGUIElement {
    public:
        CGUIColoredText(gui::IGUIEnvironment* env,
            gui::IGUIElement* parent,
            s32 id,
            const core::rect<s32>& rect)
            : gui::IGUIElement(gui::EGUIET_ELEMENT, env, parent, id, rect) {
            setTabStop(false);
            setSubElement(true);
            Font = nullptr;
            Shadow = false;
            setText(L"");
        }

        ~CGUIColoredText() noexcept override {}

        void setOverrideFont(gui::IGUIFont* f) { Font = f; Dirty = true; }
        gui::IGUIFont* getOverrideFont() const { return Font; }
        void setOverrideColor(video::SColor c) { OverrideColor = c; OverrideColorUsed = true; Dirty = true; }
        void clearOverrideColor() { OverrideColorUsed = false; Dirty = true; }
        void setTextAlignment(gui::EGUI_ALIGNMENT h, gui::EGUI_ALIGNMENT v) { AlignH = h; AlignV = v; Dirty = true; }
        void setWordWrap(bool e) { WordWrap = e; Dirty = true; }
        void setLineSpacing(int px) { LineSpacing = px; Dirty = true; }
        void setText(const wchar_t* t) override { Text = t ? t : L""; Dirty = true; }
        const wchar_t* getText() const override { return Text.c_str(); }

        void setOpacity(f32 v) { opacity = core::clamp(v, 0.f, 1.f); }
        f32 getOpacity() const { return opacity; }

        void draw() override {
            if (!IsVisible) return;
            if (!Font) Font = Environment->getSkin()->getFont();
            if (!Font) return;
            if (Dirty) layout_();

            const core::rect<s32> clip = AbsoluteClippingRect;
            const s32 lineH = Font->getDimension(L"A").Height;
            const s32 totalH = (s32)Lines.size() * lineH + (Lines.empty() ? 0 : ((int)Lines.size() - 1) * LineSpacing);

            core::position2di base(AbsoluteRect.UpperLeftCorner);
            if (AlignV == gui::EGUIA_CENTER)      base.Y += (AbsoluteRect.getHeight() - totalH) / 2;
            else if (AlignV == gui::EGUIA_LOWERRIGHT) base.Y += (AbsoluteRect.getHeight() - totalH);

            for (size_t li = 0; li < Lines.size(); ++li) {
                const auto& segs = Lines[li];
                s32 x = base.X;
                s32 lineW = LineWidths[li];
                if (AlignH == gui::EGUIA_CENTER)        x += (AbsoluteRect.getWidth() - lineW) / 2;
                else if (AlignH == gui::EGUIA_LOWERRIGHT) x += (AbsoluteRect.getWidth() - lineW);
                s32 y = base.Y + (s32)li * (lineH + LineSpacing);

                for (const auto& run : segs) {
                    const core::dimension2du d = Font->getDimension(run.text.c_str());

                    if (Shadow || run.shadow) {
                        video::SColor sc((run.color.getAlpha() * 3) / 4, 0, 0, 0);
                        sc.setAlpha((u32)((f32)sc.getAlpha() * opacity));
                        Font->draw(run.text.c_str(),
                            core::rect<s32>(x + 1, y + 1, x + 1 + (s32)d.Width, y + 1 + (s32)d.Height),
                            sc, false, false, &clip);
                    }

                    const int passes = run.bold ? 2 : 1;
                    for (int p = 0; p < passes; ++p) {
                        video::SColor c = run.color;
                        c.setAlpha((u32)((f32)c.getAlpha() * opacity));
                        Font->draw(run.text.c_str(),
                            core::rect<s32>(x + p, y,
                                x + p + (s32)d.Width, y + (s32)d.Height),
                            c, false, false, &clip);
                    }

                    if (run.underline || run.strike) {
                        auto* drv = Environment->getVideoDriver();
                        const s32 baseY = y + (s32)d.Height - 2;
                        const s32 midY = y + (s32)d.Height / 2;
                        video::SColor lc = run.color;
                        lc.setAlpha((u32)((f32)lc.getAlpha() * opacity));
                        if (run.underline) drv->draw2DRectangle(lc, core::rect<s32>(x, baseY, x + (s32)d.Width, baseY + 1), &clip);
                        if (run.strike)    drv->draw2DRectangle(lc, core::rect<s32>(x, midY, x + (s32)d.Width, midY + 1), &clip);
                    }

                    x += (s32)d.Width;
                }
            }
            IGUIElement::draw();
        }

        bool OnEvent(const SEvent& e) override { return IGUIElement::OnEvent(e); }

        void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions*) const override {
            IGUIElement::serializeAttributes(out, 0);
            out->addString("Text", Text.c_str());
            out->addBool("WordWrap", WordWrap);
            out->addInt("LineSpacing", LineSpacing);
            out->addBool("Shadow", Shadow);
        }
        void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions*) override {
            IGUIElement::deserializeAttributes(in, 0);
            setText(in->getAttributeAsStringW("Text").c_str());
            WordWrap = in->getAttributeAsBool("WordWrap");
            LineSpacing = in->getAttributeAsInt("LineSpacing");
            Shadow = in->getAttributeAsBool("Shadow");
            Dirty = true;
        }

        bool getShadow() { return Shadow; }
        void setShadow(bool s) { Shadow = s; }

    private:
        struct Run { core::stringw text; video::SColor color; bool bold = false, underline = false, strike = false, shadow = false; };

        static inline bool isHex_(wchar_t c) {
            return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') || (c >= L'A' && c <= L'F');
        }
        static inline u8 hexVal_(wchar_t c) {
            if (c >= L'0' && c <= L'9') return (u8)(c - L'0');
            if (c >= L'a' && c <= L'f') return (u8)(10 + (c - L'a'));
            return (u8)(10 + (c - L'A'));
        }
        static inline u8 parseByte_(const wchar_t* p) {
            return (u8)((hexVal_(p[0]) << 4) | hexVal_(p[1]));
        }

        void pushRun_(std::vector<Run>& runs, Run& cur) {
            if (!cur.text.empty()) {
                runs.push_back(cur);
                cur.text = L"";
            }
        }

        void layout_() {
            Lines.clear();
            LineWidths.clear();
            Dirty = false;
            if (!Font) return;

            const video::SColor baseColor = OverrideColorUsed ? OverrideColor : video::SColor(255, 255, 255, 255);

            std::vector<Run> runs;
            runs.reserve(16);

            Run cur;
            cur.color = baseColor;
            cur.shadow = Shadow;
            const wchar_t* s = Text.c_str();
            const size_t N = Text.size();

            for (size_t i = 0; i < N; ) {
                const wchar_t c = s[i];

                if (c == L'\\' && (i + 1) < N && s[i + 1] == L'<') {
                    cur.text += L'<';
                    i += 2;
                    continue;
                }

                if (c != L'<') {
                    if (c == L'\n') {
                        pushRun_(runs, cur);
                        Run nl = cur;
                        nl.text = L"\n";
                        runs.push_back(nl);
                    }
                    else {
                        cur.text += c;
                    }
                    ++i;
                    continue;
                }

                size_t j = i + 1;
                while (j < N && s[j] != L'>') ++j;
                if (j >= N) {
                    cur.text += L'<';
                    ++i;
                    continue;
                }

                core::stringw tag;
                tag.reserve((u32)(j - (i + 1)));
                for (size_t k = i + 1; k < j; ++k) tag += s[k];
                i = j + 1;
                if (tag == L"r" || tag == L"R") {
                    pushRun_(runs, cur);
                    cur = Run();
                    cur.color = baseColor;
                    cur.shadow = Shadow;
                    continue;
                }

                // <#RRGGBB> or <#RRGGBBAA>
                if ((tag.size() == 7 || tag.size() == 9) && tag[0] == L'#') {
                    bool ok = true;
                    for (u32 k = 1; k < tag.size(); ++k) ok = ok && isHex_(tag[k]);
                    if (ok) {
                        pushRun_(runs, cur);

                        const u8 r = parseByte_(tag.c_str() + 1);
                        const u8 g = parseByte_(tag.c_str() + 3);
                        const u8 b = parseByte_(tag.c_str() + 5);
                        const u8 a = (tag.size() == 9) ? parseByte_(tag.c_str() + 7) : 255;

                        cur.color = video::SColor(a, r, g, b);
                        continue;
                    }
                }

                // <b> <u> <s> <d>
                auto applyStyle = [&](bool& flag) {
                    pushRun_(runs, cur);
                    flag = true;
                    };

                if (tag == L"b" || tag == L"B") { applyStyle(cur.bold); continue; }
                if (tag == L"u" || tag == L"U") { applyStyle(cur.underline); continue; }
                if (tag == L"s" || tag == L"S") { applyStyle(cur.strike); continue; }
                if (tag == L"d" || tag == L"D") { applyStyle(cur.shadow); continue; }

                // Unknown tag -> literal
                cur.text += L'<';
                cur.text += tag;
                cur.text += L'>';
            }

            pushRun_(runs, cur);

            const s32 maxW = AbsoluteRect.getWidth();
            const bool wrap = WordWrap && maxW > 0;

            std::vector<Run> line;
            s32 currentW = 0;

            auto flushLine = [&]() {
                Lines.push_back(line);
                LineWidths.push_back(currentW);
                line.clear();
                currentW = 0;
                };

            for (const auto& r : runs) {
                if (r.text == L"\n") { flushLine(); continue; }

                if (!wrap) {
                    line.push_back(r);
                    currentW += (s32)Font->getDimension(r.text.c_str()).Width;
                    continue;
                }

                const u32 w = Font->getDimension(r.text.c_str()).Width;
                if (currentW > 0 && currentW + (s32)w > maxW) flushLine();

                line.push_back(r);
                currentW += (s32)w;
            }

            if (!line.empty()) flushLine();
        }


        f32 opacity = 1.0f;
        core::stringw Text; gui::IGUIFont* Font = nullptr; video::SColor OverrideColor{ 255,255,255,255 }; bool OverrideColorUsed = false; gui::EGUI_ALIGNMENT AlignH = gui::EGUIA_UPPERLEFT; gui::EGUI_ALIGNMENT AlignV = gui::EGUIA_UPPERLEFT; bool WordWrap = false, Shadow = true; int LineSpacing = 0; bool Dirty = true; std::vector<std::vector<Run>> Lines; std::vector<s32> LineWidths;
    };

    inline CGUIColoredText* addColoredText(gui::IGUIEnvironment* env,
        const wchar_t* text,
        const core::rect<s32>& rect,
        gui::IGUIElement* parent = nullptr,
        s32 id = -1)
    {
        if (!parent) parent = env->getRootGUIElement();
        auto* el = new CGUIColoredText(env, parent, id, rect);
        el->drop();
        el->setText(text);
        return el;
    }
};