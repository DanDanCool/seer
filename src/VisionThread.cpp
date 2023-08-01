#include "VisionThread.h"
#include "Swapchain.h"
#include "BBoxWriter.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <opencv2/opencv.hpp>
#include <cstdint>
#include <immintrin.h>

class VisionThread {
public:
	VisionThread() {
		_Run.store(false, std::memory_order_relaxed);
		_Finished.store(false, std::memory_order_relaxed);
	}

	~VisionThread() {
		Stop();
		Await();
	}

	void NextFrame(FrameData* data) {
		if (data->csize != _Size) return;
		ReadData(data);
	}

	void Run(FrameData* data) {
		_Run.store(true, std::memory_order_release);
		_Size = data->fw * data->fh;
		_Swapchain.Init(data->fw, data->fh);

		ReadData(data);

		auto task = [](VisionThread* ctx) { ctx->Task(); };
		_Thread = std::thread(task, this);
	}

	void Stop() {
		_Swapchain.Shutdown();
		_Run.store(false, std::memory_order_release);
	}

	void Await() {
		bool finished = _Finished.load(std::memory_order_acquire);
		if (!finished) {
			_Thread.join();
		}
	}

private:
	void Task();
	void ReadData(FrameData* data);

	std::thread _Thread;

	Swapchain _Swapchain;

	int _Size;
	std::atomic<bool> _Run;
	std::atomic<bool> _Finished;

	std::string _Window;
};


VisionThread* CreateVisionThread() {
	return new VisionThread();
}

void DestroyVisionThread(VisionThread* t) {
	delete t;
}

void VisionThreadUpdate(VisionThread* t, FrameData* data) {
	return t->NextFrame(data);
}

void VisionThreadRun(VisionThread* t, FrameData* data) {
	t->Run(data);
}

void VisionThreadStop(VisionThread* t) {
	t->Stop();
}

void VisionThreadAwait(VisionThread* t) {
	t->Await();
}

#include <opencv2/tracking.hpp>
#include <opencv2/tracking/tracking_legacy.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <sstream>

int SEERTestLoadFunction(int in) {
	return 7;
}

using namespace std;
using namespace cv;

void VisionThread::Task() {
	stringstream str;
	str << "tracker" << _Thread.get_id();
	_Window = str.str();

	bool run = _Run.load(memory_order_relaxed);

	double timer = (double)getTickCount();
	RNG rng;
	Scalar color(Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256)));

	string save_directory = "C:\\Users\\sunda\\Documents\\dev\\seer\\assets\\";
	VideoWriter writer(save_directory + _Window + ".mp4", VideoWriter::fourcc('H', '2', '6', '4'), 60, {_Swapchain.Width, _Swapchain.Height});
	BBoxWriter bwriter(save_directory + _Window + ".bbox");
	while (run) {
		double now = (double)getTickCount();
		{
			auto& f = _Swapchain.Present();
			std::lock_guard l(f.lock);

			auto& frame = f.frame;

			writer.write(frame);
			bwriter.Write(f.bbox);

			/*
			if (!f.bbox.empty()) {
				auto& bbox = f.bbox[0];
				Point p1(frame.cols * ((bbox.x0 + 1.0) / 2.0), frame.rows * ((-bbox.y0 + 1.0) / 2.0));
				Point p2(frame.cols * ((bbox.x1 + 1.0) / 2.0), frame.rows * ((-bbox.y1 + 1.0) / 2.0));
				rectangle(frame, p1, p2, color, 1);
			}

			float fps = getTickFrequency() / (now - timer);
			putText(frame, "FPS : " + to_string((int)fps), Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);
			*/
			imshow(_Window, frame);

			// Exit if ESC pressed.
			int k = waitKey(1);
			if (k == 27) {
				Stop();
			}
		}

		timer = now;
		run = _Run.load(memory_order_acquire);
		_Swapchain.SwapBuffers();
	}

	writer.release();
	destroyWindow(_Window);
	_Finished.store(true, std::memory_order_release);
}

void VisionThread::ReadData(FrameData* data) {
	if (!data) return;

	Frame& frame = _Swapchain.Acquire();
	std::lock_guard l(frame.lock);
	auto& m = frame.frame;

	ColorData* cdata = data->cdata;

	for (int i = 0; i < m.rows; i++) {
		for (int j = 0; j < m.cols; j++) {
			uchar* ptr = m.ptr(i, j);
			ptr[0] = cdata->b;
			ptr[1] = cdata->g;
			ptr[2] = cdata->r;
			cdata++;
		}
	}

	auto& bbox = frame.bbox;
	bbox.clear();
	for (int i = 0; i < data->bsize; i++) {
		bbox.push_back(data->bdata[i]);
	}

	_Swapchain.Submit(frame);
}
