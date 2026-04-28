#pragma once

#include <string>
class Application;

class Vec3;

namespace Module {
	namespace Physics {
		void bind(Application* app);

		namespace Bind {
			Vec3 GetGravity();
			void SetGravity(const Vec3& v);
			bool IsPaused();
			void SetPaused(bool v);
			float GetStepFactor();
			void SetStepFactor(float v);
			void SetDebugMode(int v);
			void SetIgnoreSameID(bool v);
			void SetFixedSteps(float f);
			void SetMaxSubSteps(int sub);
			int GetObjectCount();
		}
	}
}