#include <Windows.h>
#include <synchapi.h>
#include <torch/torch.h>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <string>

#include "VisionThread.h"
#include "BBoxWriter.h"

using namespace std;
using namespace cv;

struct ModelImpl : torch::nn::SequentialImpl {
	ModelImpl() {
		using namespace torch::nn;
		auto stride = torch::ExpandingArray<2>({2, 2});
		auto shape = torch::ExpandingArray<2>({-1, 128 * 7 * 7});
		push_back(Conv2d(Conv2dOptions(3, 64, 7).stride(2).padding(2)));
		push_back(Functional(torch::relu));
		push_back(Functional(torch::max_pool2d, 3, stride, 0, 1, false));
		push_back(Conv2d(Conv2dOptions(64, 128, 5).padding(2)));
		push_back(Functional(torch::relu));
		push_back(Functional(torch::max_pool2d, 3, stride, 0, 1, false));
		push_back(Conv2d(Conv2dOptions(128, 192, 3).padding(1)));
		push_back(Functional(torch::max_pool2d, 3, stride, 0, 1, false));
		push_back(Functional(torch::relu));
		push_back(Conv2d(Conv2dOptions(192, 192, 3).padding(1)));
		push_back(Functional(torch::relu));
		push_back(Conv2d(Conv2dOptions(192, 128, 3).padding(1)));
		push_back(Functional(torch::relu));
		push_back(Conv2d(Conv2dOptions(128, 128, 3).padding(1)));
		push_back(Functional(torch::relu));
		push_back(Functional(torch::max_pool2d, 3, stride, 0, 1, false));
		push_back(Functional(torch::reshape, shape));
		push_back(Dropout());
		push_back(Linear(128 * 7 * 7, 2048));
		push_back(Functional(torch::relu));
		push_back(Dropout());
		push_back(Linear(2048, 2048));
		push_back(Functional(torch::relu));
		push_back(Linear(2048, 512));
		push_back(Functional(torch::relu));
		push_back(Linear(512, 4));
	}
};
TORCH_MODULE(Model);

static BBoxData train(Model& model, torch::optim::Optimizer& optimizer, cv::Mat& frame, BBoxData& bbox) {
	std::vector<cv::Mat> channels(3);
	cv::split(frame, channels);

	auto R = torch::from_blob(channels[2].ptr(), {frame.rows, frame.cols}, torch::kUInt8);
	auto G = torch::from_blob(channels[1].ptr(), {frame.rows, frame.cols}, torch::kUInt8);
	auto B = torch::from_blob(channels[0].ptr(), {frame.rows, frame.cols}, torch::kUInt8);

	auto data = torch::cat({R, G, B}).view({3, frame.rows, frame.cols}).to(torch::kFloat);
	auto target = torch::from_blob((void*)&bbox, {1, 4}, torch::kFloat);

	auto output = model->forward(data);
	auto loss = torch::mse_loss(output, target);

	optimizer.zero_grad();
	loss.backward();
	optimizer.step();

	auto shape = output.sizes();

	auto access = output.accessor<float, 2>();
	BBoxData out;
	out.x0 = access[0][0];
	out.y0 = access[0][1];
	out.x1 = access[0][2];
	out.y1 = access[0][3];
	return out;
}

int main(int argc, char** argv) {
	ColorData* pixeldata = (ColorData*)malloc(sizeof(ColorData) * 256 * 256);
	auto* ptr = pixeldata;
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			ptr->b = 0;
			//ptr->b = (char)abs(i - j);
			ptr->g = (char)i;
			ptr->r = (char)j;
			ptr++;
		}
	}

	BBoxData bbox = { -0.5, 0.5, 0.5, -0.5 };

	Model model;
	torch::optim::SGD optimizer(model->parameters(), 0.01);

	Mat frame;
	vector<BBoxData> boxes;

	VideoCapture capture("assets/tracker3560.mp4");
	BBoxReader reader("assets/tracker3560.bbox");
	while (capture.isOpened()) {
		capture.read(frame);
		if (frame.empty()) break;
		boxes.clear();
		reader.Read(boxes);

		//BBoxData prediction = train(model, optimizer, frame, bbox);
		BBoxData& bbox = boxes[0];
		Point p1(frame.cols * ((bbox.x0 + 1.0) / 2.0), frame.rows * ((-bbox.y0 + 1.0) / 2.0));
		Point p2(frame.cols * ((bbox.x1 + 1.0) / 2.0), frame.rows * ((-bbox.y1 + 1.0) / 2.0));
		rectangle(frame, p1, p2, Scalar(0, 256, 0), 2);

		BBoxData prediction = train(model, optimizer, frame, bbox);
		Point p3(frame.cols * ((prediction.x0 + 1.0) / 2.0), frame.rows * ((-prediction.y0 + 1.0) / 2.0));
		Point p4(frame.cols * ((prediction.x1 + 1.0) / 2.0), frame.rows * ((-prediction.y1 + 1.0) / 2.0));
		rectangle(frame, p3, p4, Scalar(0, 0, 256), 2);

		imshow("bbox", frame);

		int k = waitKey(1);
		if (k == 27) {
			break;
		}
	}

	/*
	for (int j = 0; j < 1; j++) {
		VisionThread* thread = CreateVisionThread();

		FrameData data = {};
		data.cdata = pixeldata;
		data.bdata = &bbox;
		data.csize = 256 * 256;
		data.bsize = 1;
		data.fw = 256;
		data.fh = 256;

		VisionThreadRun(thread, &data);
		for (int i = 0; i < 100; i++) {
			VisionThreadUpdate(thread, &data);
			Sleep(10);
		}
		DestroyVisionThread(thread);
	}*/
}
