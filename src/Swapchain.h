#pragma once

#include <mutex>
#include <atomic>
#include <opencv2/opencv.hpp>

#include <Windows.h>

#include "VisionThread.h"

const int FRAME_COUNT = 2;

struct Semaphore {
	HANDLE sem;
	int val;

	Semaphore(int max) {
		sem = CreateSemaphore(NULL, 0, max, NULL);
		DWORD err = GetLastError();
		assert(!err);
		val = -1;
	}

	~Semaphore() {
		CloseHandle(sem);
		sem = NULL;
	}

	void Acquire() {
		WaitForSingleObject(sem, INFINITE);
	}

	void Release() {
		LONG count = -1;
		ReleaseSemaphore(sem, 1, &count);
		DWORD err = GetLastError();
		if (count != -1) {
			val = count;
		}
	}
};

struct Frame {
	cv::Mat frame;
	std::vector<BBoxData> bbox;
	std::mutex lock;
};

class Swapchain {
public:
	Swapchain()
		: _Signals{{1}, {1}}
	{
		_BackIndex.store(0, std::memory_order_relaxed);
		_Present.store(0, std::memory_order_relaxed);
	}

	~Swapchain() {
	}

	void Init(int x, int y) {
		Width = x;
		Height = y;
		for (int i = 0; i < FRAME_COUNT; i++) {
			_Frames[i].frame.create(y, x, CV_8UC3);
		}
	}

	void Shutdown() {
		_Signals[0].Release();
		_Signals[1].Release();
	}

	Frame& Present() {
		int idx = _Present.load(std::memory_order_acquire);
		return _Frames[idx];
	}

	void SwapBuffers() {
		int idx = _Present.load(std::memory_order_acquire);
		_Present.store(!idx, std::memory_order_release);
		_Signals[!idx].Acquire();
	}

	Frame& Acquire() {
		int idx = _BackIndex.load(std::memory_order_acquire);
		return _Frames[idx];
	}

	void Submit(Frame& frame) {
		int idx = _BackIndex.load(std::memory_order_acquire);
		assert(&_Frames[idx] == &frame);
		_BackIndex.store(!idx, std::memory_order_release);
		_Signals[idx].Release();
	}

	int Width, Height;
private:
	Frame _Frames[FRAME_COUNT];
	Semaphore _Signals[FRAME_COUNT];
	std::atomic<int> _BackIndex;
	std::atomic<int> _Present;
};
