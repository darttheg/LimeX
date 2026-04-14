#include "Modules/ModuleWeb.h"

#include "Application.h"
#include "WebManager.h"
#include "Objects/Event.h"

#include <sol/sol.hpp>

static Application* a;
static WebManager* w;

void Module::Web::bind(Application* app) {
	a = app;
	w = app->GetWebManager();
	sol::state& lua = app->GetLuaState();

	// Module Lime.Web
	sol::table module = lua["Lime"]["Web"].get_or_create<sol::table>();

	w->LimeOnGetComplete = std::make_shared<Event>();
	w->LimeOnGetFail = std::make_shared<Event>();
	w->LimeOnDownloadComplete = std::make_shared<Event>();
	w->LimeOnDownloadFail = std::make_shared<Event>();

	// Field Event onGetComplete, Event called by Lime when a GET request has successfully completed.
	// Params number status, string content
	module["onGetComplete"] = w->LimeOnGetComplete;

	// Field Event onGetFail, Event called by Lime when a GET request has failed.
	// Params number status
	module["onGetFail"] = w->LimeOnGetFail;

	// Field Event onDownloadComplete, Event called by Lime when a download has successfully completed.
	// Params number status, string path
	module["onDownloadComplete"] = w->LimeOnDownloadComplete;

	// Field Event onDownloadFail, Event called by Lime when a download has failed.
	// Params number status
	module["onDownloadFail"] = w->LimeOnDownloadFail;

	// Sends an HTTP GET request to `url`. See `Lime.Web.onGetComplete` and `Lime.Web.onGetFail`.
	// Params string url
	// Returns void
	module.set_function("get", &Module::Web::Bind::Get);

	// Downloads a file from `url` and saves it to `path`. See `Lime.Web.onDownloadComplete` and `Lime.Web.onDownloadFail`.
	// Params string url, string path
	// Returns void
	module.set_function("download", &Module::Web::Bind::DownloadFile);

	// Cancels the ongoing download, if any.
	// Returns void
	module.set_function("cancelDownload", &Module::Web::Bind::CancelDownload);

	// Returns the download progress from 0.0 to 1.0.
	// Returns number
	module.set_function("getDownloadProgress", &Module::Web::Bind::GetProgress);

	// Returns the download speed in bytes per second.
	// Returns number
	module.set_function("getDownloadSpeed", &Module::Web::Bind::GetSpeed);

	// Returns true if there is an ongoing download.
	// Returns boolean
	module.set_function("isDownloading", &Module::Web::Bind::IsDownloading);

	// Returns true if an ongoing download is extracting its content.
	// Returns boolean
	module.set_function("isExtracting", &Module::Web::Bind::IsExtracting);

	// Sets the timeout in milliseconds for HTTP requests.
	// Params number ms
	// Returns void
	module.set_function("setTimeout", &Module::Web::Bind::SetTimeout);

	// End Module
}

// Functions

void Module::Web::Bind::Get(const std::string& url) {
	w->get(url);
}

void Module::Web::Bind::DownloadFile(const std::string& url, const std::string& outPath) {
	w->downloadFile(url, outPath);
}

void Module::Web::Bind::CancelDownload() {
	w->cancelDownload();
}

float Module::Web::Bind::GetProgress() {
	return w->getProgress();
}

float Module::Web::Bind::GetSpeed() {
	return w->getSpeed();
}

bool Module::Web::Bind::IsDownloading() {
	return w->isDownloading();
}

bool Module::Web::Bind::IsExtracting() {
	return w->isExtracting();
}

void Module::Web::Bind::SetTimeout(int ms) {
	w->setTimeout(ms);
}