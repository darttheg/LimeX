#pragma once

class Application;
class Vec2;
class Vec3;

namespace irrklang {
	class ISoundEngine;
	class ISound;
}

class SoundManager {
public:
	SoundManager(Application* owner);
	~SoundManager() = default;

	bool Init();
	bool Update();
private:
	irrklang::ISoundEngine* i_sound = nullptr;

	struct Vec3S { float x, y, z; };
	
	bool useManualListenerVel = false;
	Vec3S listenerVel;
};