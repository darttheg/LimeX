#include "Modules/ModuleSound.h"

#include "Application.h"
#include "SoundManager.h"
#include "Objects/Vec2.h"
#include "Objects/Vec3.h"

#include <sol/sol.hpp>

static SoundManager* s;

void Module::Sound::bind(Application* app) {
	s = app->GetSoundManager();
	sol::state& lua = app->GetLuaState();

	// Module Lime.Audio
	sol::table module = lua["Lime"]["Audio"].get_or_create<sol::table>();

	// Returns the listener velocity factor.
	// Returns number
	module.set_function("getVelocityFactor", &Module::Sound::Bind::GetVelFactor);

	// Sets the factor to scale listener velocity by. Listener velocity is calculated with the last and current active `Camera` positions. In the event that the active `Camera` is swapped, listener velocity will be 0 on the first frame automatically.
	// Params number factor
	// Returns void
	module.set_function("setVelocityFactor", &Module::Sound::Bind::SetVelFactor);

	// Returns the application's main volume.
	// Returns number
	module.set_function("getMainVolume", &Module::Sound::Bind::GetMainVolume);

	// Sets the application's main volume.
	// Params number volume
	// Returns void
	module.set_function("setMainVolume", &Module::Sound::Bind::SetMainVolume);

	// Sets all `Sound` objects to paused or unpaused.
	// Params boolean paused
	// Returns void
	module.set_function("setAllSoundsPaused", &Module::Sound::Bind::SetAllPaused);

	// Sets the default minimum listening distance for new sounds.
	// Params number distance
	// Returns void
	module.set_function("setDefaultMinimumDistance", &Module::Sound::Bind::SetDefaultMinDistance);

	// Sets the default maximum listening distance for new sounds.
	// Params number distance
	// Returns void
	module.set_function("setDefaultMaximumDistance", &Module::Sound::Bind::SetDefaultMaxDistance);

	// Determines unfocused window behavior in regards to application volume.
	// Params boolean mute
	// Returns void
	module.set_function("setMuteWhileUnfocused", &Module::Sound::Bind::SetMuteUnfocus);

	// End Module
}

// Functions

float Module::Sound::Bind::GetVelFactor() {
	return s->getVelFactor();
}

void Module::Sound::Bind::SetVelFactor(float v) {
	s->setVelFactor(v);
}

int Module::Sound::Bind::GetMainVolume() {
	return s->getMainVolume();
}

void Module::Sound::Bind::SetMainVolume(int v) {
	s->setMainVolume(v);
}

void Module::Sound::Bind::SetAllPaused(bool v) {
	s->setAllSoundsPaused(v);
}

void Module::Sound::Bind::SetMuteUnfocus(bool v) {
	s->setMuteUnfocus(v);
}

void Module::Sound::Bind::StopAll() {
	s->stopAllSounds();
}

int Module::Sound::Bind::GetLoadedCount() {
	return s->getLoadedSoundsCount();
}

void Module::Sound::Bind::SetDefaultMaxDistance(float max) {
	s->setDefaultMax(max);
}

void Module::Sound::Bind::SetDefaultMinDistance(float min) {
	s->setDefaultMin(min);
}

void Module::Sound::Bind::SetDopplerEffectParameters(float dopplerFactor, float distanceFactor) {
	s->setDopplerEffectParameters(dopplerFactor, distanceFactor);
}