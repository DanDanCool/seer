#include "VisionThread.h"
#include "Swapchain.h"
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

	void NextFrame(ColorData* data, int size) {
		if (size != _Size) return;
		ReadData(data);
	}

	void Run(ColorData* data, int x, int y) {
		_Run.store(true, std::memory_order_release);
		_Size = x * y;
		_Swapchain.Init(x, y);

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
	void ReadData(ColorData* data);

	std::thread _Thread;

	Swapchain _Swapchain;

	int _Size;
	std::atomic<bool> _Run;
	std::atomic<bool> _Finished;

	std::string _Window;
};

int TestLoadFunction(int in) {
	printf("hello %d\n", in);

	return 1;
}


VisionThread* CreateVisionThread() {
	return new VisionThread();
}

void DestroyVisionThread(VisionThread* t) {
	delete t;
}

void VisionThreadUpdate(VisionThread* t, ColorData* data, int size) {
	return t->NextFrame(data, size);
}

void VisionThreadRun(VisionThread* t, ColorData* data, int x, int y) {
	t->Run(data, x, y);
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

using namespace cv;
using namespace std;

void VisionThread::Task() {
	stringstream str;
	str << "tracker" << _Thread.get_id();
	_Window = str.str();

	bool run = _Run.load(memory_order_relaxed);

	double timer = (double)getTickCount();
	while (run) {
		double now = (double)getTickCount();

		{
			auto& [frame, lock] = _Swapchain.Present();
			std::lock_guard l(lock);

			float fps = getTickFrequency() / (now - timer);
			putText(frame, "FPS : " + to_string((int)fps), Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);
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

	destroyWindow(_Window);
	_Finished.store(true, std::memory_order_release);
}

void VisionThread::ReadData(ColorData* data) {
	if (!data) return;

	auto& [frame, lock] = _Swapchain.Acquire();
	std::lock_guard l(lock);

	for (int i = 0; i < frame.rows; i++) {
		for (int j = 0; j < frame.cols; j++) {
			uchar* ptr = frame.ptr(i, j);
			ptr[0] = data->b;
			ptr[1] = data->g;
			ptr[2] = data->r;
			data++;
		}
	}

	_Swapchain.Submit(frame);
}
