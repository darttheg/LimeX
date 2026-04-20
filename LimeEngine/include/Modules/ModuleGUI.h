#pragma once

#include <string>
class Application;

namespace Module {
	namespace GUI {
		void bind(Application* app);

		namespace Bind {
			bool render();
			std::string embedFont(const std::string& path);
			void setDefaultFont(const std::string& name);
			bool isFontEmbedded(const std::string& name);
			void setQuality(int q);
			std::string embedTTF(const std::string& ttfPath, int size, bool aa = true);
			std::string embedTTF(const std::string& ttfPath, int size, const std::string& name, bool aa = true);
			void clearFocus();
		}
	}
}