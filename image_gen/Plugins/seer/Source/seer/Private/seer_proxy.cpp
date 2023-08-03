#include "seer_proxy.h"
#include "VisionThread.h"
#include "Editor.h"

SeerProxy::SeerProxy() {
	_Run = false;
}

SeerProxy::~SeerProxy() {
	VisionThreadStop();
}

void SeerProxy::Run(TArray<FColor>& pixels, TArray<FVector4f>& boxes, int x, int y) {
	_Run = true;

	FrameData data = {};
	data.cdata = (ColorData*)pixels.GetData();
	data.bdata = (BBoxData*)boxes.GetData();
	data.csize = pixels.Num();
	data.bsize = boxes.Num();
	data.fw = x;
	data.fh = y;
	VisionThreadRun(&data);
}

void SeerProxy::NextFrame(TArray<FColor>& pixels, TArray<FVector4f>& boxes) {
	FrameData data = {};
	data.cdata = (ColorData*)pixels.GetData();
	data.bdata = (BBoxData*)boxes.GetData();
	data.csize = pixels.Num();
	data.bsize = boxes.Num();
	VisionThreadUpdate(&data);
}

bool SeerProxy::IsRunning() {
	return _Run;
}
