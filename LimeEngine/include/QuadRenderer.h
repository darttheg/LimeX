#pragma once
#include <cstdint>
#include <irrlicht.h>

class DebugConsole;

class QuadRenderer {
public:
	void init(irr::video::IVideoDriver* driver);
	void setInternalResolution(std::uint32_t w, std::uint32_t h);
	void setWindowResolution(std::uint32_t w, std::uint32_t h);
	void setMatchWindowRender(bool m) { matchWR = m; }
	void setClearColor(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a);

	void beginInternal();
	void endInternal();
	void presentToWindow();

	void prepareToRecreateRt();
	int getTimeRt() { return timeToRecreate; }

	irr::video::ITexture* getRenderTarget() const { return rt; }

	irr::core::recti getViewport() { return vp; }

private:
	irr::video::IVideoDriver* driver = nullptr;

	std::uint32_t resW = 640, resH = 480;
	std::uint32_t winW = 640, winH = 480;
	irr::video::SColor clear = { 255, 0, 0, 0 };
	irr::video::ITexture* rt = nullptr;

	irr::video::S3DVertex qVerts[4];
	irr::u16 qIndices[6];
	irr::video::SMaterial qMat;
	bool qBuilt = false;
	bool matchWR = true;

	irr::core::recti vp;

	int timeToRecreate = 0;
	bool didRecreate = true;

	void buildQuad();
	void recreateRt();
};