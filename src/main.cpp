#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/tracking/tracking_legacy.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>

#include "VisionThread.h"

#include <Windows.h>
#include <synchapi.h>

using namespace cv;
using namespace std;

Ptr<Tracker> createTracker(const string& name) {
	Ptr<Tracker> tracker;
	if (name == "DaSiamRPN") {
		tracker = TrackerDaSiamRPN::create();
	}
	if (name == "Nano") {
		TrackerNano::Params params;
		params.backbone = "assets/nanotrack_backbone_sim.onnx";
		params.neckhead = "assets/nanotrack_head_sim.onnx";
		tracker = TrackerNano::create(params);
	}
	if (name == "GOTURN") {
		TrackerGOTURN::Params params;
		params.modelBin = "assets/goturn.caffemodel";
		params.modelTxt = "assets/goturn.prototxt";
		tracker = TrackerGOTURN::create(params);
	}
	if (name == "MIL") {
		tracker = TrackerMIL::create();
	}

	return tracker;
}

void randomColors(vector<Scalar>& colors, int n) {
	RNG rng(0);
	for (int i = 0; i < n; i++) {
		colors.push_back(Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)));
	}
}

int main(int argc, char** argv) {
	string trackerTypes[] = { "DaSiamRPN", "Nano", "GOTURN", "MIL" };

	int idx = 1;
	//scanf("%d", &idx);
	// Create a tracker
	string trackerType = trackerTypes[idx];

	string filename = "assets/test1.mkv";

	// Read video
	VideoCapture video(filename);

	// Exit if video is not opened
	if (!video.isOpened()) {
		cout << "Could not read video file" << endl;
		return 1;
	}

	// Read first frame
	Mat frame;
	bool ok = video.read(frame);

	// Define initial bounding box
	vector<Rect> bbox;

	bool showCrosshair = true;
	bool fromCenter = false;

	ColorData* data = (ColorData*)malloc(sizeof(ColorData) * 256 * 256);
	auto* ptr = data;
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			ptr->b = 0;
			//ptr->b = (char)abs(i - j);
			ptr->g = (char)i;
			ptr->r = (char)j;
			ptr++;
		}
	}


	for (int j = 0; j < 10; j++) {
		VisionThread* thread = CreateVisionThread();
		VisionThreadRun(thread, data, 256, 256);
		for (int i = 0; i < 60; i++) {
			VisionThreadUpdate(thread, data, 256 * 256);
			Sleep(100);
		}
		DestroyVisionThread(thread);
	}
}
