#pragma once
#include <irrlicht.h>
#include <vector>

using namespace irr;
using namespace scene;

namespace irr::scene {
    class CTextAnchorSceneNode : public scene::ISceneNode {
    public:
        CTextAnchorSceneNode(scene::ISceneNode* parent,
            scene::ISceneManager* smgr,
            gui::IGUIEnvironment* guienv,
            gui::IGUIElement* textElem,
            s32 id = -1)
            : scene::ISceneNode(parent, smgr, id), Env(guienv), TextElem(textElem)
        {
            AutomaticCullingState = scene::EAC_OFF;
        }

        void OnAnimate(u32 timeMs) override {
            ISceneNode::OnAnimate(timeMs);
            if (!Env || !TextElem) return;

            auto* cam = SceneManager->getActiveCamera();
            auto* coll = SceneManager->getSceneCollisionManager();
            if (!cam || !coll) return;

            const core::vector3df pos3d = getAbsolutePosition();
            core::position2di p2d = coll->getScreenCoordinatesFrom3DPosition(pos3d, cam);

            if (HideBehind) {
                const core::vector3df toPt = pos3d - cam->getAbsolutePosition();
                const core::vector3df camDir = cam->getTarget() - cam->getAbsolutePosition();
                if (toPt.dotProduct(camDir) <= 0.f) {
                    TextElem->setVisible(false);
                    return;
                }
            }

            TextElem->setVisible(true);
            p2d += PixelOffset;

            core::rect<s32> r = TextElem->getRelativePosition();
            const s32 w = r.getWidth();
            const s32 h = r.getHeight();

            if (Centered) {
                r.UpperLeftCorner = p2d - core::position2di(w / 2, h / 2);
            }
            else {
                r.UpperLeftCorner = p2d;
            }
            r.LowerRightCorner = r.UpperLeftCorner + core::dimension2di(w, h);

            TextElem->setRelativePosition(r);
        }

        void render() override {}
        const core::aabbox3d<f32>& getBoundingBox() const override { return Box; }

    private:
        gui::IGUIEnvironment* Env = nullptr;
        gui::IGUIElement* TextElem = nullptr;

        core::position2di PixelOffset{ 0,0 };
        bool Centered = true;
        bool HideBehind = true;

        core::aabbox3d<f32> Box;
    };
}