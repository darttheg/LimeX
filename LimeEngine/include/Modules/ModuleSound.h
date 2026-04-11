#pragma once

#include <string>
class Application;

class Vec2;
class Vec3;
class Vec4;

namespace Module {
	namespace Sound {
		void bind(Application* app);

		namespace Bind {
			float GetVelFactor();
			void SetVelFactor(float v);
			int GetMainVolume();
			void SetMainVolume(int v);
			void SetAllPaused(bool v);
			void SetMuteUnfocus(bool v);
			void StopAll();
			int GetLoadedCount();
			void SetDefaultMinDistance(float min);
			void SetDefaultMaxDistance(float max);
			void SetDopplerEffectParameters(float dopplerFactor, float distanceFactor);
			bool PreloadSound(const std::string& path);
			// bool AppendSourceToAlias(const Sound& src, const std::string& name);
			// bool RemoveAliasAndSources(const std::string& name);
		}
	}
}