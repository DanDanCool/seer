#include "seer_proxy.h"
#include "VisionThread.h"
#include "Editor.h"

SeerProxy::SeerProxy() {
	_Run = false;
	_Thread = CreateVisionThread();
}

SeerProxy::~SeerProxy() {
	if (!_Thread) return;
	DestroyVisionThread(_Thread);
}

void SeerProxy::Run(TArray<FColor>& pixels, TArray<FVector4f>& boxes, int x, int y) {
	if (!_Thread) return;
	_Run = true;

	FrameData data = {};
	data.cdata = (ColorData*)pixels.GetData();
	data.bdata = (BBoxData*)boxes.GetData();
	data.csize = pixels.Num();
	data.bsize = boxes.Num();
	data.fw = x;
	data.fh = y;
	VisionThreadRun(_Thread, &data);
}

void SeerProxy::NextFrame(TArray<FColor>& pixels, TArray<FVector4f>& boxes) {
	if (!_Thread) return;
	FrameData data = {};
	data.cdata = (ColorData*)pixels.GetData();
	data.bdata = (BBoxData*)boxes.GetData();
	data.csize = pixels.Num();
	data.bsize = boxes.Num();
	VisionThreadUpdate(_Thread, &data);
}

bool SeerProxy::IsRunning() {
	return _Run;
}
