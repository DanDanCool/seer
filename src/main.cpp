#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/tracking/tracking_legacy.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>

#include <torch/torch.h>

#include "VisionThread.h"

#include <Windows.h>
#include <synchapi.h>

using namespace cv;
using namespace std;

struct Model : torch::nn::Module {
	Model()
	//: conv1(1, 4, 3)
	//, conv2(4, 4, 3)
	: fc0(64, 256)
	, fc1(256, 256)
	, fc2(256, 256)
	, fc3(256, 64)
	, fc4(64, 1)
	, sig() {
		//register_module("conv1", conv1);
		//register_module("conv2", conv2);
		register_module("fc0", fc0);
		register_module("fc1", fc1);
		register_module("fc2", fc2);
		register_module("fc3", fc3);
		register_module("fc4", fc4);
		register_module("sig", sig);
	}

	torch::Tensor forward(torch::Tensor x) {
		//x = torch::relu(conv1->forward(x));
		//x = torch::relu(conv2->forward(x));
		//x = x.view({-1, 256});
		//
		x = torch::flatten(x);
		x = torch::relu(fc0->forward(x));
		x = torch::relu(fc1->forward(x));
		x = torch::relu(fc2->forward(x));
		x = torch::relu(fc3->forward(x));
		x = fc4->forward(x);
		x = sig->forward(x);
		return x;
	}

	//torch::nn::Conv2d conv1;
	//torch::nn::Conv2d conv2;
	torch::nn::Linear fc0;
	torch::nn::Linear fc1;
	torch::nn::Linear fc2;
	torch::nn::Linear fc3;
	torch::nn::Linear fc4;
	torch::nn::Sigmoid sig;
};

int main(int argc, char** argv) {
	/*
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
	*/

	if (torch::cuda::is_available()) {
		cout << "cuda available\n\n";
	}

	Model model;
	torch::optim::SGD optimizer(model.parameters(), 0.1);

	for (int epoch = 0; epoch < 50; epoch++) {
		torch::Tensor loss, target, pred;
		for (int batch = 0; batch < 128; batch++) {
			optimizer.zero_grad();

			torch::Tensor data = torch::rand({8, 8});
			int count = 0;
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					double d = data[i][j].item<double>() > 0.5 ? 1.0 : 0.0;
					data[i][j] = d;
					count += d == 1.0;
				}
			}

			target = torch::tensor(vector(1, count < 32 ? 0.0 : 1.0));
			pred = model.forward(data);

			loss = torch::binary_cross_entropy(pred, target);
			loss.backward();
			optimizer.step();
		}

		cout << "\nEpoch: " << epoch << " loss: " << loss.item<float>() << endl;
		cout << "\ntarget: " << target << "\n\nprediction: " << pred << endl;
	}

	int correct = 0;
	for (int batch = 0; batch < 128; batch++) {
		torch::Tensor data = torch::rand({8, 8});
		int count = 0;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				double d = data[i][j].item<double>() > 0.5 ? 1.0 : 0.0;
				data[i][j] = d;
				count += d == 1.0;
			}
		}

		double target = count < 32 ? 0.0 : 1.0;
		auto pred = model.forward(data);
		double intermediate = pred[0].item<double>() > 0.5 ? 1.0 : 0.0;
		correct += intermediate == target;
	}

	double acc = correct / 128.0;
	cout << "accuracy: " << acc << endl;

	/*
	for (int j = 0; j < 10; j++) {
		VisionThread* thread = CreateVisionThread();
		VisionThreadRun(thread, data, 256, 256);
		for (int i = 0; i < 10; i++) {
			VisionThreadUpdate(thread, data, 256 * 256);
			Sleep(100);
		}
		DestroyVisionThread(thread);
	} */
}
