#pragma once
#include <string>
#include <vector>

class Application;
class Vec2;
class Vec3;

namespace irrklang {
	class ISoundEngine;
	class ISound;
	class ISoundSource;
}

namespace irr::scene {
	class ICameraSceneNode;
	class ISceneNode;
}

struct SoundSourceOnNode {
	irrklang::ISound* sound;
	irr::scene::ISceneNode* parent;
};

class SoundManager {
public:
	SoundManager(Application* owner);
	~SoundManager() = default;

	bool Init();
	bool Update(float dt);
	bool guardSoundCheck(std::string msg = "");

	float getVelFactor() { return velFactor; }
	void setVelFactor(float f) { velFactor = f; }

	int getMainVolume();
	void setMainVolume(int v);
	void setAllSoundsPaused(bool v);
	void stopAllSounds();
	int getLoadedSoundsCount();
	void setDefaultMin(float min);
	void setDefaultMax(float max);
	void setMuteUnfocus(bool v);
	void setDopplerEffectParameters(float dopplerFactor, float distanceFactor);
	void warnGarbageCollection(const std::string& path);
	// bool appendSourceToAlias(const Sound& src, const std::string& name);
	// bool removeAliasAndSources(const std::string& name);

	irrklang::ISoundSource* createSoundSource(const std::string& path, int type = 0);
	void unloadSound(irrklang::ISoundSource* src);
	irrklang::ISound* play(irrklang::ISoundSource* src, bool td, bool loops, bool sfx);
	bool attachSoundToNode(irrklang::ISound* sound, irr::scene::ISceneNode* parent);
private:
	irrklang::ISoundEngine* i_sound = nullptr;

	struct Vec3S { float x, y, z; };
	Vec3S lastCamPos{ 0,0,0 };
	bool firstVel = true;
	float velFactor = 1.0f;
	float mainVol = 1.0f;
	bool muteUnfocus = false;

	irr::scene::ICameraSceneNode* last = nullptr;

	std::vector<SoundSourceOnNode> soundNodePairs;
};