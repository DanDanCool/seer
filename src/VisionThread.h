#pragma once

#ifdef SEER_EXPORT
#define SEER_API __declspec(dllexport)
#else
#define SEER_API __declspec(dllimport)
#endif

struct ColorData {
	char b;
	char g;
	char r;
	char a;
};

class VisionThread;

SEER_API int TestLoadFunction(int in);

SEER_API VisionThread* CreateVisionThread();
SEER_API void DestroyVisionThread(VisionThread* t);
SEER_API void VisionThreadUpdate(VisionThread* t, ColorData* data, int size);
SEER_API void VisionThreadRun(VisionThread* t, ColorData* data, int x, int y);
SEER_API void VisionThreadStop(VisionThread* t);
SEER_API void VisionThreadAwait(VisionThread* t);
