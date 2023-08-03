#include "VisionThread.h"
#include "Swapchain.h"
#include "BBoxWriter.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <opencv2/opencv.hpp>
#include <cstdint>
#include <immintrin.h>

static VisionThread* g_VisionThread = nullptr;

class VisionThread {
public:
	VisionThread() : _Signal(1) {
		_Run.store(false, std::memory_order_relaxed);
		_Finished.store(false, std::memory_order_relaxed);
	}

	~VisionThread() {
		_Finished.store(true, std::memory_order_release);
		_Signal.Release();
		_Thread.join();
	}

	void RunThread() {
		auto task = [](VisionThread* ctx) { ctx->Task(); };
		_Thread = std::thread(task, this);
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

		_Signal.Release();
	}

	void Stop() {
		_Swapchain.Shutdown();
		_Run.store(false, std::memory_order_release);
	}

	void Task();
	void ReadData(FrameData* data);

private:
	std::thread _Thread;

	Semaphore _Signal;
	Swapchain _Swapchain;

	int _Size;
	std::atomic<bool> _Run;
	std::atomic<bool> _Finished;

	std::string _Window;
};

void CreateVisionThread() {
	if (g_VisionThread) return;
	g_VisionThread = new VisionThread();
	g_VisionThread->RunThread();
}

void DestroyVisionThread() {
	delete g_VisionThread;
	g_VisionThread = nullptr;
}

void VisionThreadUpdate(FrameData* data) {
	return g_VisionThread->NextFrame(data);
}

void VisionThreadRun(FrameData* data) {
	g_VisionThread->Run(data);
}

void VisionThreadStop() {
	g_VisionThread->Stop();
}

#include <opencv2/tracking.hpp>
#include <opencv2/tracking/tracking_legacy.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <sstream>
#include <chrono>
#include <iomanip>

int SEERTestLoadFunction(int in) {
	return 7;
}

using namespace std;
using namespace cv;

void VisionThread::Task() {
	auto task = [&]() {
		{
			auto now = time(nullptr);
			auto tm = *localtime(&now);
			stringstream str;
			str << put_time(&tm, "%F-%H-%M-%S");
			_Window = str.str();
		}

		bool run = _Run.load(memory_order_relaxed);

		string save_directory = "C:\\Users\\sunda\\Documents\\dev\\seer\\assets\\";
		VideoWriter writer(save_directory + _Window + ".mp4", VideoWriter::fourcc('H', '2', '6', '4'), 60, {_Swapchain.Width, _Swapchain.Height});
		BBoxWriter bwriter(save_directory + _Window + ".bbox");
		while (run) {
			double now = (double)getTickCount();
			{
				auto& f = _Swapchain.Present();
				std::lock_guard l(f.lock);

				writer.write(f.frame);
				bwriter.Write(f.bbox);
				imshow(_Window, f.frame);

				// Exit if ESC pressed.
				int k = waitKey(1);
				if (k == 27) Stop();
			}

			run = _Run.load(memory_order_acquire);
			_Swapchain.SwapBuffers();
		}

		writer.release();
		destroyWindow(_Window);
	};

	bool finished = _Finished.load(std::memory_order_acquire);
	while (!finished) {
		_Signal.Acquire();
		finished = _Finished.load(std::memory_order_acquire);
		if (!finished) task();
	}
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
