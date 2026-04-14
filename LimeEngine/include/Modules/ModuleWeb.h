#pragma once

#include <sol/forward.hpp>
#include <string>

class Application;

namespace Module {
	namespace Web {
		void bind(Application* app);

		namespace Bind {
			void Get(const std::string& url);
			void DownloadFile(const std::string& url, const std::string& outPath);
			void CancelDownload();
			float GetProgress();
			float GetSpeed();
			bool IsDownloading();
			bool IsExtracting();
			void SetTimeout(int ms);
		}
	}
}