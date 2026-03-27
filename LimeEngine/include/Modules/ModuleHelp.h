#pragma once

#include <string>
#include <sol/forward.hpp>
class Application;

namespace Module {
	namespace Help {
		void bind(Application* app);

		namespace Bind {
			// Noise
			void setNoiseSeed(int seed);
			int getNoiseSeed();
			void setNoiseOctaves(int oc);
			int getNoiseOctaves();
			float getNoiseValue1D(float x);
			float getNoiseValue2D(float x, float y);
			float getNoiseValue3D(float x, float y, float z);
		}
	}
}