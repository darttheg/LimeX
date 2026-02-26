#include "QuadRenderer.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

void QuadRenderer::init(irr::video::IVideoDriver* d)
{
    driver = d;

    buildQuad();
    recreateRt();
}

void QuadRenderer::setInternalResolution(std::uint32_t w, std::uint32_t h)
{
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
    driver->OnResize(irr::core::dimension2du((irr::u32)w, (irr::u32)h));
}

void QuadRenderer::setClearColor(std::uint32_t br, std::uint32_t bg, std::uint32_t bb, std::uint32_t ba) {
    clear.set(ba, br, bg, bb);
}

void QuadRenderer::beginInternal()
{
    if (!driver) return;

    if (rt && timeToRecreate >= 0 && !didRecreate) {
        didRecreate = true;
        recreateRt();
    }

    if (timeToRecreate >= 0) timeToRecreate = 0;

    if (!rt) recreateRt();

    driver->setRenderTarget(rt, true, true, clear);

    irr::s32 oW = matchWR ? winW : resW;
    irr::s32 oH = matchWR ? winH : resH;
    driver->setViewPort(irr::core::rect<irr::s32>(0, 0, oW, oH));
    // std::cout << std::to_string(oW) << ", " << std::to_string(oH) << "\n";
    // Scaling window smaller than render resolution causes weird viewport issues, but no numbers seem off.
}

void QuadRenderer::endInternal()
{
    if (!driver)
        return;

    driver->setRenderTarget(nullptr, false, false);
    timeToRecreate += 1;
}

void QuadRenderer::presentToWindow()
{
    if (!driver || !rt)
        return;

    if (!matchWR) {
        const float sx = winW / (float)resW;
        const float sy = winH / (float)resH;
        const float s = std::min(sx, sy);
        const int dstW = (int)std::lround(resW * s);
        const int dstH = (int)std::lround(resH * s);

        const int x0 = (winW - dstW) / 2;
        const int y0 = (winH - dstH) / 2;

        driver->setViewPort({ x0, y0, x0 + dstW, y0 + dstH });
    } else {
        driver->setViewPort({ 0, 0, (int)winW, (int)winH });
    }

    irr::core::matrix4 I; I.makeIdentity();
    driver->setTransform(irr::video::ETS_WORLD, I);
    driver->setTransform(irr::video::ETS_VIEW, I);
    driver->setTransform(irr::video::ETS_PROJECTION, I);

    qMat.setTexture(0, rt);

    driver->setMaterial(qMat);

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

void QuadRenderer::buildQuad()
{
    if (qBuilt) return;

    qVerts[0] = irr::video::S3DVertex(-1.f, -1.f, 0.f, 0, 0, 1, 0xFFFFFFFF, 0.f, 1.f);
    qVerts[1] = irr::video::S3DVertex(1.f, -1.f, 0.f, 0, 0, 1, 0xFFFFFFFF, 1.f, 1.f);
    qVerts[2] = irr::video::S3DVertex(1.f, 1.f, 0.f, 0, 0, 1, 0xFFFFFFFF, 1.f, 0.f);
    qVerts[3] = irr::video::S3DVertex(-1.f, 1.f, 0.f, 0, 0, 1, 0xFFFFFFFF, 0.f, 0.f);

    qIndices[0] = 0; qIndices[1] = 1; qIndices[2] = 2;
    qIndices[3] = 0; qIndices[4] = 2; qIndices[5] = 3;

    qMat = irr::video::SMaterial();
    qMat.Lighting = false;
    qMat.ZBuffer = false;
    qMat.ZWriteEnable = false;
    qMat.BackfaceCulling = false;
    qMat.MaterialType = irr::video::EMT_SOLID;

    qMat.setFlag(irr::video::EMF_BILINEAR_FILTER, false);
    qMat.setFlag(irr::video::EMF_TRILINEAR_FILTER, false);
    qMat.setFlag(irr::video::EMF_ANISOTROPIC_FILTER, false);
    qMat.setFlag(irr::video::EMF_USE_MIP_MAPS, false);
    qMat.setFlag(irr::video::EMF_ANTI_ALIASING, irr::video::E_ANTI_ALIASING_MODE::EAAM_OFF);

    qMat.TextureLayer[0].TextureWrapU = irr::video::ETC_CLAMP_TO_EDGE;
    qMat.TextureLayer[0].TextureWrapV = irr::video::ETC_CLAMP_TO_EDGE;

    qBuilt = true;
}

void QuadRenderer::recreateRt()
{
    if (!driver) return;

    if (rt) {
        rt->drop();
        driver->removeTexture(rt);
        rt = nullptr;
    }

    irr::s32 oW = matchWR ? winW : resW;
    irr::s32 oH = matchWR ? winH : resH;

    const irr::core::dimension2du res(oW, oH);
    rt = driver->addRenderTargetTexture(res, "rt", irr::video::ECF_A8R8G8B8);

    if (rt) {
        rt->grab();
        qMat.setTexture(0, rt);
    }
}