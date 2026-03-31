#pragma once

#include <string>

extern "C" { struct lua_State; }
class SoundManager;
class RenderHelper;

#include <sol/optional.hpp>
#include <sol/forward.hpp>
#include <Interfaces/Object3D.h>

namespace irrklang {
	class ISound;
	class ISoundSource;
}

namespace irr::scene {
	class ISceneNode;
	class IBillboardSceneNode;
}

class DebugAxisPlaneNode;
class Vec3;

class SoundSource {
public:
	SoundSource();
	SoundSource(const std::string& path, int type = 0);

	bool play(bool td = false);
	void stop();
	bool isPlaying();
	bool getPaused();
	void setPaused(bool v);
	bool getLooping();
	void setLooping(bool v);
	float getMinDist();
	void setMinDist(float f);
	float getMaxDist();
	void setMaxDist(float f);
	void setVolume(float f);
	float getVolume();
	void setPitch(float f);
	float getPitch();
	void setPan(float f);
	float getPan();
	int getPlayPosition();
	void setPlayPosition(int ms);
	int getPlayLength();
	void setVelocity(const Vec3& vel);
	Vec3 getVelocity();
	void setPosition(const Vec3& vel);
	Vec3 getPosition();
	bool getDoSFX();
	void setDoSFX(bool v);
	
	bool getDebug();
	void setDebug(bool v);

	bool loadFromFile(const std::string& pat, int type = 0);

	bool attachTo(sol::optional<Object3D*> parent); // 3D
	bool isAttached();
	std::string getPath();

	void clearEffects();
	bool addDistortionEffect(float gain = -18.0f, float edge = 15.0f);
	bool addEchoEffect(float wetDry = 50.0f, float feedback = 50.0f, float delay = 250.0f);
	bool addReverbEffect(float inputGain = 0.0f, float mix = 0.0f, float time = 1000.0f, float freqRatio = 0.001f);
	bool addCompressionEffect(float threshold = -20.0f, float ratio = 3.0f);
	bool addParamEqEffect(float fCenter = 8000.0f, float fBandwidth = 12.0f, float fGain = 0.0f);

	void collected();

	// Destroy will just destroy this object and the sound. Purge will do the same but also unload it.
	sol::object destroy();
	sol::object purge();

private:
	irrklang::ISound* cur = nullptr;
	irrklang::ISoundSource* src = nullptr;

	struct Vec3S { float x, y, z; };

	bool loops = false;
	bool is3D = false;
	bool doSFX = false;
	float vol = 100.0f;
	float minDist = 1.0f;
	float maxDist = 5000.0f;
	Vec3S pos = Vec3S{ 0,0,0 };
	irr::scene::ISceneNode* parent = nullptr;

	irr::scene::IBillboardSceneNode* dVisual = nullptr;
	DebugAxisPlaneNode* dAxis = nullptr;
};

namespace Object::SoundSourceBind {
	void bind(lua_State* ls, SoundManager* sou, RenderHelper* renh);
}