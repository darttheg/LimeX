#pragma once

#include <sol/forward.hpp>
#include <string>

class DebugConsole;
class Application;
class Renderer;

class Vec2;
class Vec3;
class Vec4;
class Texture;
class Mesh;
class HitResult;
class ShaderMaterial;

namespace Module {
	namespace Scene {
		void bind(Application* app);

		namespace Bind {
			int GetObjectCount();
			void SetBackgroundColor(const Vec4& color);
			void SetFogColor(const Vec4& color);
			void SetFogPlanes(const Vec2& planes);
			void SetAmbientColor(const Vec4& color);
			void SetShadowColor(const Vec4& color);
			void SetLightManagementType(int type);
			void SetTextureCreationQuality(int quality);
			void SetMatchRenderResToWindow(bool v);
			Texture GetErrorTexture();
			void SetRenderSize(const Vec2& size);
			void SetRenderQuality(int q);
			void SetActivelyRendering(bool v);
			bool GetActivelyRendering();
			void SetLightManagerType(int type);
			void SetPostProcessingShader(const ShaderMaterial& sm);
			void ClearPostProcessingShader();
			Vec2 ConvertToScreenSpace(const Vec3& pos);
			sol::table FireRaycast(const Vec3& start, const Vec3& end, float life = 0);
			sol::table FireScreenRaycast(const Vec2& start, float len, float life = 0);

			Mesh CreateCubeMesh(const Vec3& size);
			Mesh CreateSphereMesh(float r, int polyCount = 8);
			Mesh CreateCylinderMesh(float r, float l, int polyCount = 8, bool closed = true);
			Mesh CreatePlaneMesh(const Vec2& tileSize, const Vec2& tileCount);
			Mesh CreatePlaneMesh(const Vec2& tileSize, const Vec2& tileCount, const Vec2& texRepeat);

			bool PreloadMesh(const std::string path);
			bool PurgeMesh(const std::string path);
			bool PreloadTexture(const std::string path);
			bool PurgeTexture(const std::string path);
		}
	}
}