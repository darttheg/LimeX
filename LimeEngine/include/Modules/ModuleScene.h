#pragma once

#include <sol/forward.hpp>
#include <string>

class DebugConsole;
class Application;
class Renderer;

class Vec2;
class Vec3;
class Vec4;
class Image;
//class Mesh;

namespace Module {
	namespace Scene {
		void bind(Application* app);

		namespace Bind {
			bool RenderScene(bool clearBackBuffer = false, bool clearZBuffer = true);
			int GetObjectCount();
			void SetBackgroundColor(const Vec4& color);
			void SetFogColor(const Vec4& color);
			void SetFogPlanes(const Vec2& planes);
			void SetAmbientColor(const Vec4& color);
			void SetShadowColor(const Vec4& color);
			void SetLightManagementType(int type);
			void SetTextureCreationQuality(int quality);
			void SetMatchRenderResToWindow(bool v);
			Image GetErrorImage();
			void SetRenderSize(const Vec2& size);

			//Mesh CreateCubeMesh(int scale);
			//Mesh CreateRectangleMesh(const Vec3& scale);
			//Mesh CreateSphereMesh(int scale);
		}
	}
}