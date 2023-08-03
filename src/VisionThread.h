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

SEER_API int SEERTestLoadFunction(int in);

SEER_API void CreateVisionThread();
SEER_API void DestroyVisionThread();
SEER_API void VisionThreadUpdate(FrameData* data);
SEER_API void VisionThreadRun(FrameData* data);
SEER_API void VisionThreadStop();
