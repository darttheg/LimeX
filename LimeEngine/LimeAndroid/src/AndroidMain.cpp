// Android entry point, the LimePlayer equivalent.
// Game files are copied from the APK's assets/game to internal storage, which becomes the working directory.

#include "LimeEngine.h"
#include "LimeAndroid.h"

#include <android_native_app_glue.h>
#include <android/asset_manager.h>
#include <android/log.h>

#include <jni.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>

extern "C" {
	extern const unsigned char lime_resources_zip[];
	extern const unsigned long lime_resources_zip_size;
}

static android_app* gApp = nullptr;
static std::string gDataDir;

android_app* LimeAndroid::getApp() { return gApp; }
const char* LimeAndroid::getDataDir() { return gDataDir.c_str(); }

const unsigned char* LimeAndroid::getResourcesZip(unsigned long* size) {
	*size = lime_resources_zip_size;
	return lime_resources_zip;
}

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Lime", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Lime", __VA_ARGS__)

// On-screen keyboard (LimeActivity)
static std::mutex gKeyboardMutex;
static std::vector<LimeAndroid::KeyboardInput> gKeyboardInput;
static jmethodID gSetKeyboardVisible = nullptr;

static void JNICALL onText(JNIEnv* env, jclass, jstring text) {
	if (!text) return;
	const jchar* chars = env->GetStringChars(text, nullptr);
	LimeAndroid::KeyboardInput in;
	in.text.assign((const char16_t*)chars, (size_t)env->GetStringLength(text));
	env->ReleaseStringChars(text, chars);
	std::lock_guard<std::mutex> lock(gKeyboardMutex);
	gKeyboardInput.push_back(std::move(in));
}

static void JNICALL onKey(JNIEnv*, jclass, jint keyCode) {
	LimeAndroid::KeyboardInput in;
	in.keyCode = keyCode;
	std::lock_guard<std::mutex> lock(gKeyboardMutex);
	gKeyboardInput.push_back(std::move(in));
}

void LimeAndroid::takeKeyboardInput(std::vector<KeyboardInput>& out) {
	std::lock_guard<std::mutex> lock(gKeyboardMutex);
	out.swap(gKeyboardInput);
	gKeyboardInput.clear();
}

static JNIEnv* attachThread(android_app* app) {
	JNIEnv* env = nullptr;
	if (app->activity->vm->AttachCurrentThread(&env, nullptr) != JNI_OK) return nullptr;
	return env;
}

static void registerActivityNatives(android_app* app) {
	JNIEnv* env = attachThread(app);
	if (!env) return;
	jclass cls = env->GetObjectClass(app->activity->clazz);
	const JNINativeMethod methods[] = {
		{ "nativeOnText", "(Ljava/lang/String;)V", (void*)onText },
		{ "nativeOnKey", "(I)V", (void*)onKey },
	};
	if (env->RegisterNatives(cls, methods, 2) != JNI_OK) {
		env->ExceptionClear();
	} else {
		gSetKeyboardVisible = env->GetMethodID(cls, "setKeyboardVisible", "(Z)V");
		if (!gSetKeyboardVisible) env->ExceptionClear();
	}
	env->DeleteLocalRef(cls);
}

void LimeAndroid::showSoftKeyboard(irr::IrrlichtDevice*, bool show) {
	if (!gApp) return;
	JNIEnv* env = gSetKeyboardVisible ? attachThread(gApp) : nullptr;
	if (env) {
		env->CallVoidMethod(gApp->activity->clazz, gSetKeyboardVisible, (jboolean)show);
		if (env->ExceptionCheck()) env->ExceptionClear();
	} else if (show) {
		ANativeActivity_showSoftInput(gApp->activity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_FORCED);
	} else {
		ANativeActivity_hideSoftInput(gApp->activity, ANATIVEACTIVITY_HIDE_SOFT_INPUT_NOT_ALWAYS);
	}
}

static bool readAsset(AAssetManager* mgr, const std::string& name, std::string& out) {
	AAsset* asset = AAssetManager_open(mgr, name.c_str(), AASSET_MODE_BUFFER);
	if (!asset) {
		__android_log_print(ANDROID_LOG_ERROR, "Lime", "Could not open asset %s", name.c_str());
		return false;
	}
	const off_t length = AAsset_getLength(asset);
	out.resize((size_t)length);
	const int read = length > 0 ? AAsset_read(asset, &out[0], (size_t)length) : 0;
	AAsset_close(asset);
	if (read != length)
		__android_log_print(ANDROID_LOG_ERROR, "Lime", "Read %d of %ld bytes from asset %s", read, (long)length, name.c_str());
	return read == length;
}

static bool makeDirs(const std::string& path) {
	for (size_t i = 1; i <= path.size(); ++i) {
		if (i == path.size() || path[i] == '/') {
			const std::string part = path.substr(0, i);
			if (mkdir(part.c_str(), 0770) != 0 && errno != EEXIST) return false;
		}
	}
	return true;
}

static bool copyAssetToFile(AAssetManager* mgr, const std::string& assetName, const std::string& destination) {
	AAsset* asset = AAssetManager_open(mgr, assetName.c_str(), AASSET_MODE_STREAMING);
	if (!asset) return false;

	const size_t slash = destination.find_last_of('/');
	if (slash != std::string::npos && !makeDirs(destination.substr(0, slash))) {
		AAsset_close(asset);
		return false;
	}

	FILE* f = fopen(destination.c_str(), "wb");
	if (!f) {
		AAsset_close(asset);
		return false;
	}
	char buffer[64 * 1024];
	int n;
	bool ok = true;
	while ((n = AAsset_read(asset, buffer, sizeof(buffer))) > 0) {
		if (fwrite(buffer, 1, (size_t)n, f) != (size_t)n) { ok = false; break; }
	}
	fclose(f);
	AAsset_close(asset);
	return ok && n >= 0;
}

// Skipped if this build id was already extracted
static bool extractGame(AAssetManager* mgr, const std::string& dataDir) {
	std::string manifest;
	if (!readAsset(mgr, "lime/manifest.txt", manifest)) {
		LOGE("The APK has no lime/manifest.txt");
		return false;
	}

	std::istringstream lines(manifest);
	std::string buildId;
	std::getline(lines, buildId);
	if (!buildId.empty() && buildId.back() == '\r') buildId.pop_back();

	const std::string stampPath = dataDir + "/.lime_build";
	std::string installed;
	{
		std::ifstream stamp(stampPath);
		std::getline(stamp, installed);
	}
	if (!buildId.empty() && installed == buildId) return true;

	LOGI("Extracting game files (build %s)", buildId.c_str());
	std::string path;
	int count = 0;
	while (std::getline(lines, path)) {
		if (!path.empty() && path.back() == '\r') path.pop_back();
		if (path.empty()) continue;
		if (!copyAssetToFile(mgr, "game/" + path, dataDir + "/" + path)) {
			LOGE("Could not extract %s", path.c_str());
			return false;
		}
		++count;
	}

	std::ofstream stamp(stampPath, std::ios::trunc);
	stamp << buildId << "\n";
	LOGI("Extracted %d files", count);
	return true;
}

static void waitCommand(android_app* app, int32_t cmd) {
	(void)app;
	(void)cmd;
}

static int32_t ignoreInput(android_app*, AInputEvent*) {
	return 0;
}

static void pumpUntil(android_app* app, bool (*done)(android_app*)) {
	while (!done(app)) {
		int events = 0;
		android_poll_source* source = nullptr;
		const int id = ALooper_pollOnce(-1, nullptr, &events, (void**)&source);
		if (id >= 0 && source) source->process(app, source);
	}
}

static bool hasWindowOrDestroyed(android_app* app) { return app->window != nullptr || app->destroyRequested; }
static bool destroyed(android_app* app) { return app->destroyRequested != 0; }

void android_main(android_app* app) {
	gApp = app;
	app->onAppCmd = waitCommand;
	app->onInputEvent = ignoreInput;
	registerActivityNatives(app);

	gDataDir = app->activity->internalDataPath ? app->activity->internalDataPath : "";
	if (gDataDir.empty() || !makeDirs(gDataDir) || chdir(gDataDir.c_str()) != 0) {
		LOGE("No usable internal storage directory");
		ANativeActivity_finish(app->activity);
		pumpUntil(app, destroyed);
		app->activity->vm->DetachCurrentThread();
		exit(0);
	}

	AAssetManager* assets = app->activity->assetManager;
	std::string package;
	if (!extractGame(assets, gDataDir) || !readAsset(assets, "lime/app.limepkg", package)) {
		LOGE("The game could not be loaded from the APK");
		ANativeActivity_finish(app->activity);
		pumpUntil(app, destroyed);
		app->activity->vm->DetachCurrentThread();
		exit(0);
	}

	pumpUntil(app, hasWindowOrDestroyed);
	if (!app->destroyRequested) {
		const char* argv[] = { "lime" };
		LimeHandle handle = Lime_Create();
		int result = Lime_Run(handle, 1, argv, package.data(), package.size());
		Lime_End(handle);
		LOGI("Lime exited with code %d", result);

		app->onAppCmd = waitCommand;
		app->onInputEvent = ignoreInput;
		ANativeActivity_finish(app->activity);
		pumpUntil(app, destroyed);
	}

	// Engine state is global, so the next launch gets a fresh process
	app->activity->vm->DetachCurrentThread();
	exit(0);
}
