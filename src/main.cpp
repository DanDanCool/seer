#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/tracking/tracking_legacy.hpp>
#include <opencv2/core/ocl.hpp>

using namespace cv;
using namespace std;

// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()

Ptr<legacy::Tracker> createTracker(const string& name) {
	Ptr<legacy::Tracker> tracker;
	if (name == "BOOSTING")
		tracker = legacy::TrackerBoosting::create();
	if (name == "MIL")
		tracker = legacy::TrackerMIL::create();
	if (name == "KCF")
		tracker = legacy::TrackerKCF::create();
	if (name == "TLD")
		tracker = legacy::TrackerTLD::create();
	if (name == "MEDIANFLOW")
		tracker = legacy::TrackerMedianFlow::create();
	//if (trackerType == "GOTURN")
	//	tracker = legacy::TrackerGOTURN::create();
	if (name == "MOSSE")
		tracker = legacy::TrackerMOSSE::create();
	if (name == "CSRT")
		tracker = legacy::TrackerCSRT::create();

	return tracker;
}

void randomColors(vector<Scalar>& colors, int n) {
	RNG rng(0);
	for (int i = 0; i < n; i++) {
		colors.push_back(Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)));
	}
}

int main(int argc, char** argv) {
	// List of tracker types in OpenCV 3.4.1
	string trackerTypes[8] = { "BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN", "MOSSE", "CSRT" };
	// vector <string> trackerTypes(types, std::end(types));

	int idx = 0;
	scanf("%d", &idx);
	// Create a tracker
	string trackerType = trackerTypes[idx];

	string filename = "assets/street.mp4";

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

	selectROIs("MultiTracker", frame, bbox, showCrosshair, fromCenter);

	if (bbox.empty()) return 0;

	vector<Scalar> colors;
	randomColors(colors, bbox.size());

	Ptr<legacy::MultiTracker> tracker = legacy::MultiTracker::create();
	for (auto& box : bbox) {
		tracker->add(createTracker(trackerTypes[idx]), frame, Rect2d(box));
	}

	while (video.read(frame)) {
		// Start timer
		double timer = (double)getTickCount();

		video >> frame;
		if (frame.empty()) break;

		tracker->update(frame);

		auto& objects = tracker->getObjects();
		for (int i = 0; i < objects.size(); i++) {
			rectangle(frame, objects[i], colors[i], 2, 1);
		}

		// Calculate Frames per second (FPS)
		float fps = getTickFrequency() / ((double)getTickCount() - timer);

		// Display tracker type on frame
		putText(frame, trackerType + " Tracker", Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);

		// Display FPS on frame
		putText(frame, "FPS : " + SSTR(int(fps)), Point(100, 50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);

		// Display frame.
		imshow("MultiTracker", frame);

		// Exit if ESC pressed.
		int k = waitKey(1);
		if (k == 27) {
			break;
		}

	}
}
