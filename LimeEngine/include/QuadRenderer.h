#pragma once
#include <cstdint>
#include <memory>
#include <irrlicht.h>

class DebugConsole;
class IrrShaderMaterial;

class QuadRenderer {
public:
	void init(irr::video::IVideoDriver* driver, irr::gui::IGUIEnvironment* gui);
	void setInternalResolution(std::uint32_t w, std::uint32_t h);
	void setWindowResolution(std::uint32_t w, std::uint32_t h);
	void setMatchWindowRender(bool m);
	void setClearColor(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a);

	void setPostProcessingShader(int shaderID, IrrShaderMaterial* cb);
	void setPostProcessingShaderGUI(int shaderID, IrrShaderMaterial* cb);
	void clearPostProcessingShader();
	void clearPostProcessingShaderGUI();

	void beginInternal();
	void beginGUIPass();
	void endInternal();
	void presentToWindow();

	void prepareToRecreateRt();
	int getTimeRt() { return timeToRecreate; }

	irr::video::ITexture* getRenderTarget() const { return rtScene; }

	irr::core::recti getViewport() { return vp; }

	void setSceneRenderQuality(int q);
	void setIntegerScaling(bool enable);

	bool ppxActive() { return ppxType != -1 || gppxType != -1; }
	
	int getPpxType() { return ppxType; }
	irr::video::E_MATERIAL_TYPE getMatType() { return qMat.MaterialType; }

	irr::video::ITexture* getSceneTexture() { return rtScene; }
	irr::video::ITexture* getGUITexture() { return rtGUI; }
	irr::video::ITexture* getUserTexture() { return rtUser; }
	irr::video::ITexture* getDepthTexture() { return rtDepth; }
	void setUserTexture(irr::video::ITexture* tex) { rtUser = tex; }
	void clearUserTexture() { rtUser = nullptr; }
	void clearUsedTextures(irr::video::ITexture* tex); // If texture is purged, check that it isn't used here.
private:
	irr::video::IVideoDriver* driver = nullptr;
	irr::gui::IGUIEnvironment* gui = nullptr;

	std::uint32_t resW = 640, resH = 480;
	std::uint32_t winW = 640, winH = 480;
	irr::video::SColor clear = { 255, 0, 0, 0 };
	irr::video::ITexture* rtScene = nullptr;
	irr::video::ITexture* rtGUI = nullptr;
	irr::video::ITexture* rtUser = nullptr;
	irr::video::ITexture* rtDepth = nullptr;

	irr::video::S3DVertex qVerts[4];
	irr::u16 qIndices[6];
	irr::video::SMaterial qMat;
	irr::video::SMaterial qBlendMat;
	bool matchWR = true;
	bool integerScaling = false;
	IrrShaderMaterial* ppxCB = nullptr;
	IrrShaderMaterial* gppxCB = nullptr;

	irr::core::recti vp;

	int timeToRecreate = 0;
	bool didRecreate = true;
	bool highQuality = true;

	void buildQuad();
	void recreateRt();
	void setVp();

	int ppxType = -1;
	int gppxType = -1;
};