#pragma once
#include <cstdint>
#include <memory>
#include <irrlicht.h>

class DebugConsole;
class ShaderMaterial;

class QuadRenderer {
public:
	void init(irr::video::IVideoDriver* driver);
	void setInternalResolution(std::uint32_t w, std::uint32_t h);
	void setWindowResolution(std::uint32_t w, std::uint32_t h);
	void setMatchWindowRender(bool m);
	void setClearColor(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a);

	void setPostProcessingShader(int shaderID);
	void clearPostProcessingShader();

	void beginInternal();
	void beginGUIPass();
	void endInternal();
	void presentToWindow();

	void prepareToRecreateRt();
	int getTimeRt() { return timeToRecreate; }

	irr::video::ITexture* getRenderTarget() const { return rtScene; }

	irr::core::recti getViewport() { return vp; }

	void setSceneRenderQuality(int q);

private:
	irr::video::IVideoDriver* driver = nullptr;

	std::uint32_t resW = 640, resH = 480;
	std::uint32_t winW = 640, winH = 480;
	irr::video::SColor clear = { 255, 0, 0, 0 };
	irr::video::ITexture* rtScene = nullptr;
	irr::video::ITexture* rtGUI = nullptr;

	irr::video::S3DVertex qVerts[4];
	irr::u16 qIndices[6];
	irr::video::SMaterial qMat;
	irr::video::SMaterial qBlendMat;
	bool matchWR = true;

	irr::core::recti vp;

	int timeToRecreate = 0;
	bool didRecreate = true;
	bool highQuality = true;

	void buildQuad();
	void recreateRt();
	void setVp();

	int ppxType = -1;
};