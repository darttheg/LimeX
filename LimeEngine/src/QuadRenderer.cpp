#include "QuadRenderer.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

void QuadRenderer::init(irr::video::IVideoDriver* d, irr::gui::IGUIEnvironment* g) {
    driver = d;
    gui = g;

    buildQuad();
    recreateRt();
}

void QuadRenderer::setInternalResolution(std::uint32_t w, std::uint32_t h) {
    w = std::max<std::uint32_t>(1u, w);
    h = std::max<std::uint32_t>(1u, h);

    if (resW == w && resH == h) return;

    resW = w;
    resH = h;

    recreateRt();
}

void QuadRenderer::setWindowResolution(std::uint32_t w, std::uint32_t h) {
    winW = w;
    winH = h;
    if (driver) driver->OnResize(irr::core::dimension2du((irr::u32)w, (irr::u32)h));
}

void QuadRenderer::setMatchWindowRender(bool m) {
    if (matchWR == m) return;

    matchWR = m;
    recreateRt();
}

void QuadRenderer::setClearColor(std::uint32_t br, std::uint32_t bg, std::uint32_t bb, std::uint32_t ba) {
    clear.set(ba, br, bg, bb);
}

void QuadRenderer::setPostProcessingShader(int shaderID) {
    ppxType = shaderID;
}

void QuadRenderer::clearPostProcessingShader() {
    ppxType = -1;
}

void QuadRenderer::beginInternal()
{
    if (!driver) return;

    if (rtScene && timeToRecreate >= 0 && !didRecreate) {
        didRecreate = true;
        recreateRt();
    }

    if (timeToRecreate >= 0) timeToRecreate = 0;

    if (!rtScene) recreateRt();

    driver->setRenderTarget(rtScene, true, true, clear);

    irr::s32 oW = matchWR ? winW : resW;
    irr::s32 oH = matchWR ? winH : resH;
    driver->setViewPort(irr::core::rect<irr::s32>(0, 0, oW, oH));
}

void QuadRenderer::beginGUIPass() {
    driver->setRenderTarget(rtGUI, true, true);
}

void QuadRenderer::endInternal()
{
    if (!driver) return;

    driver->setRenderTarget(nullptr, false, false);
    timeToRecreate += 1;
}

void QuadRenderer::presentToWindow()
{
    if (!driver || !rtScene)
        return;

    setVp();

    // Scene
    qMat.setTexture(0, rtScene);
    qMat.setFlag(irr::video::EMF_BILINEAR_FILTER, highQuality);
    qMat.setFlag(irr::video::EMF_ANTI_ALIASING, highQuality ? irr::video::E_ANTI_ALIASING_MODE::EAAM_SIMPLE : irr::video::E_ANTI_ALIASING_MODE::EAAM_OFF);
    
    if (ppxType >= 0)
        qMat.MaterialType = static_cast<irr::video::E_MATERIAL_TYPE>(ppxType);
    else
        qMat.MaterialType = irr::video::EMT_SOLID;

    driver->setMaterial(qMat);

    driver->drawVertexPrimitiveList(
        qVerts, 4,
        qIndices, 2,
        irr::video::EVT_STANDARD,
        irr::scene::EPT_TRIANGLES,
        irr::video::EIT_16BIT
    );

    // User Texture
    if (rtUser) {
        qMat.setTexture(0, rtUser);
        driver->setMaterial(qMat);
        driver->drawVertexPrimitiveList(
            qVerts, 4,
            qIndices, 2,
            irr::video::EVT_STANDARD,
            irr::scene::EPT_TRIANGLES,
            irr::video::EIT_16BIT
        );
    }

    // GUI
    qBlendMat.setTexture(0, rtGUI);
    driver->setMaterial(qBlendMat);

    driver->drawVertexPrimitiveList(
        qVerts, 4,
        qIndices, 2,
        irr::video::EVT_STANDARD,
        irr::scene::EPT_TRIANGLES,
        irr::video::EIT_16BIT
    );
}

void QuadRenderer::prepareToRecreateRt() {
    timeToRecreate = -1;
    didRecreate = false;
}

void QuadRenderer::setSceneRenderQuality(int q) {
    if (q == 0) highQuality = false; else highQuality = true;
}

void QuadRenderer::clearUsedTextures(irr::video::ITexture* tex) {
    // if (tex == rtScene) rtScene = nullptr;
    // if (tex == rtGUI) rtGUI = nullptr;
    if (tex == rtUser) rtUser = nullptr;
}

void QuadRenderer::buildQuad()
{
    qVerts[0] = irr::video::S3DVertex(-1.f, -1.f, 0.f, 0, 0, 1, 0xFFFFFFFF, 0.f, 1.f);
    qVerts[1] = irr::video::S3DVertex(1.f, -1.f, 0.f, 0, 0, 1, 0xFFFFFFFF, 1.f, 1.f);
    qVerts[2] = irr::video::S3DVertex(1.f, 1.f, 0.f, 0, 0, 1, 0xFFFFFFFF, 1.f, 0.f);
    qVerts[3] = irr::video::S3DVertex(-1.f, 1.f, 0.f, 0, 0, 1, 0xFFFFFFFF, 0.f, 0.f);

    qIndices[0] = 0; qIndices[1] = 1; qIndices[2] = 2;
    qIndices[3] = 0; qIndices[4] = 2; qIndices[5] = 3;

    qMat = irr::video::SMaterial();
    qMat.Lighting = false;
    qMat.FogEnable = false;
    qMat.ZBuffer = false;
    qMat.ZWriteEnable = false;
    qMat.BackfaceCulling = false;
    qMat.MaterialType = irr::video::EMT_SOLID;
    qMat.setFlag(irr::video::EMF_BILINEAR_FILTER, true);
    qMat.setFlag(irr::video::EMF_TRILINEAR_FILTER, false);
    qMat.setFlag(irr::video::EMF_ANISOTROPIC_FILTER, false);
    qMat.setFlag(irr::video::EMF_USE_MIP_MAPS, false);
    qMat.setFlag(irr::video::EMF_ANTI_ALIASING, irr::video::E_ANTI_ALIASING_MODE::EAAM_LINE_SMOOTH);

    qBlendMat = irr::video::SMaterial();
    qBlendMat.Lighting = false;
    qBlendMat.FogEnable = false;
    qBlendMat.ZBuffer = false;
    qBlendMat.ZWriteEnable = false;
    qBlendMat.BackfaceCulling = false;
    qBlendMat.MaterialType = irr::video::EMT_ONETEXTURE_BLEND;
    qBlendMat.setFlag(irr::video::EMF_BILINEAR_FILTER, false);
    qBlendMat.setFlag(irr::video::EMF_TRILINEAR_FILTER, false);
    qBlendMat.setFlag(irr::video::EMF_ANISOTROPIC_FILTER, false);
    qBlendMat.setFlag(irr::video::EMF_USE_MIP_MAPS, false);
    qBlendMat.setFlag(irr::video::EMF_ANTI_ALIASING, irr::video::E_ANTI_ALIASING_MODE::EAAM_OFF);
    qBlendMat.MaterialTypeParam = irr::video::pack_textureBlendFunc(
        irr::video::EBF_ONE,
        irr::video::EBF_ONE_MINUS_SRC_ALPHA,
        irr::video::EMFN_MODULATE_1X,
        irr::video::EAS_TEXTURE
    );

    qBlendMat.TextureLayer[0].TextureWrapU = irr::video::ETC_CLAMP_TO_EDGE;
    qBlendMat.TextureLayer[0].TextureWrapV = irr::video::ETC_CLAMP_TO_EDGE;

    qMat.TextureLayer[0].TextureWrapU = irr::video::ETC_CLAMP_TO_EDGE;
    qMat.TextureLayer[0].TextureWrapV = irr::video::ETC_CLAMP_TO_EDGE;
}

void QuadRenderer::recreateRt() {
    if (!driver) return;

    if (rtScene) {
        rtScene->drop();
        driver->removeTexture(rtScene);
        rtScene = nullptr;
    }

    if (rtGUI) {
        rtGUI->drop();
        driver->removeTexture(rtGUI);
        rtGUI = nullptr;
    }

    irr::s32 oW = matchWR ? winW : resW;
    irr::s32 oH = matchWR ? winH : resH;

    const irr::core::dimension2du res(oW, oH);
    rtScene = driver->addRenderTargetTexture(res, "rtScene", irr::video::ECF_A8R8G8B8);
    rtGUI = driver->addRenderTargetTexture(res, "rtGUI", irr::video::ECF_A8R8G8B8);

    if (rtScene) {
        rtScene->grab();
        qMat.setTexture(0, rtScene);
    }

    if (rtGUI) rtGUI->grab();
}

#undef min
#undef max
void QuadRenderer::setVp() {
    if (!driver) return;

    if (!matchWR) {
        const float sx = winW / (float)resW;
        const float sy = winH / (float)resH;
        const float s = std::min(sx, sy);
        const int dstW = (int)std::lround(resW * s);
        const int dstH = (int)std::lround(resH * s);

        const int x0 = std::max(((int)winW - dstW) / 2, 0);
        const int y0 = std::max(((int)winH - dstH) / 2, 0);

        vp = { x0, y0, x0 + dstW, y0 + dstH };
        driver->setViewPort(vp);
    }
    else {
        vp = { 0, 0, (int)winW, (int)winH };
        driver->setViewPort(vp);
    }

    irr::core::matrix4 proj;
    proj.makeIdentity();

    if (driver->getDriverType() == irr::video::EDT_DIRECT3D9) {
        float xOff = -1.0f / winW;
        float yOff = 1.0f / winH;
        proj.setTranslation(irr::core::vector3df(xOff, yOff, 0.0f));
    }

    irr::core::matrix4 I;
    I.makeIdentity();
    driver->setTransform(irr::video::ETS_WORLD, I);
    driver->setTransform(irr::video::ETS_VIEW, I);
    driver->setTransform(irr::video::ETS_PROJECTION, proj);
}
