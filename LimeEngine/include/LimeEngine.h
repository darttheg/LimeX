#pragma once

#ifdef LIMEENGINE_EXPORT
	#define LIMEAPI __declspec(dllexport)
#else
	#define LIMEAPI __declspec(dllimport)
#endif

extern "C" {
	typedef void* LimeHandle;
	LIMEAPI LimeHandle Lime_Create();
	LIMEAPI int Lime_Run(LimeHandle handle, const void* data, size_t size);
	LIMEAPI void Lime_End(LimeHandle handle);
}