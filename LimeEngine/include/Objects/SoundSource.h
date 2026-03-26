#pragma once

#include <string>

extern "C" { struct lua_State; }
class SoundManager;

#include <sol/optional.hpp>
#include <sol/forward.hpp>
#include <Interfaces/Object3D.h>

namespace irrklang {
	class ISound;
	class ISoundSource;
}

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
	bool getDoSFX();
	void setDoSFX(bool v);
	
	bool getDebug();
	void setDebug(bool v);

	bool loadFromFile(const std::string& pat, int type = 0);

	void attachTo(sol::optional<Object3D*> parent); // 3D
	std::string getPath();

	// Destroy will just destroy this object and the sound. Purge will do the same but also unload it.
	sol::object destroy();
	sol::object purge();

private:
	irrklang::ISound* cur = nullptr;
	irrklang::ISoundSource* src = nullptr;

	struct Vec3S { float x, y, z; };

	bool loops = false;
	bool attached = false;
	bool doSFX = false;
	Vec3S pos = Vec3S{ 0,0,0 };
};

namespace Object::SoundSourceBind {
	void bind(lua_State* ls, SoundManager* sou);
}