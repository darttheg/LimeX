#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Application;
class Vec2;
class Vec3;
struct SoundMini;
struct ma_engine;

namespace irr::scene {
	class ICameraSceneNode;
	class ISceneNode;
}

struct SoundData {
	std::string path;
	bool stream = false;
};

struct SoundSourceOnNode {
	std::weak_ptr<SoundMini> sound;
	irr::scene::ISceneNode* parent;
};

class SoundManager {
public:
	SoundManager(Application* owner);
	~SoundManager() = default;

	bool Init();
	bool Update(float dt);
	void Shutdown();
	bool guardSoundCheck(std::string msg = "");

	float getVelFactor() { return velFactor; }
	void setVelFactor(float f) { velFactor = f; }
	float getDefaultMin() { return defaultMin; }
	float getDefaultMax() { return defaultMax; }
	float getDistanceFactor() { return distanceFactor; }

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
	bool preloadSound(const std::string& path);

	SoundData* createSoundSource(const std::string& path, int type = 0);
	void unloadSound(SoundData* src);
	std::shared_ptr<SoundMini> play(SoundData* src, bool td, bool loops);
	bool attachSoundToNode(const std::shared_ptr<SoundMini>& sound, irr::scene::ISceneNode* parent);
	void detachSoundFromNode(const SoundMini* sound);
private:
	std::unique_ptr<ma_engine> engine;
	bool ready = false;

	std::unordered_map<std::string, std::unique_ptr<SoundData>> sources;
	std::vector<std::shared_ptr<SoundMini>> soundMinis;

	struct Vec3S { float x, y, z; };
	Vec3S lastCamPos{ 0,0,0 };
	bool firstVel = true;
	float velFactor = 0.0f;
	float mainVol = 1.0f;
	bool muteUnfocus = false;
	float defaultMin = 1.0f;
	float defaultMax = 100000000.0f;
	float dopplerFactor = 1.0f;
	float distanceFactor = 1.0f;

	irr::scene::ICameraSceneNode* last = nullptr;

	std::vector<SoundSourceOnNode> soundNodePairs;
};