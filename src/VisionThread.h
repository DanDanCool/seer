#pragma once

#ifdef SEER_EXPORT
#define SEER_API __declspec(dllexport)
#else
#define SEER_API __declspec(dllimport)
#endif

#include <cstdint>

struct ColorData {
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
};

struct BBoxData {
	float x0;
	float y0;
	float x1;
	float y1;
};

struct FrameData {
	ColorData* cdata;
	BBoxData* bdata;
	int csize;
	int bsize;
	int fw; // width
	int fh; // height
};

class VisionThread;

SEER_API int TestLoadFunction(int in);

SEER_API VisionThread* CreateVisionThread();
SEER_API void DestroyVisionThread(VisionThread* t);
SEER_API void VisionThreadUpdate(VisionThread* t, FrameData* data);
SEER_API void VisionThreadRun(VisionThread* t, FrameData* data);
SEER_API void VisionThreadStop(VisionThread* t);
SEER_API void VisionThreadAwait(VisionThread* t);
