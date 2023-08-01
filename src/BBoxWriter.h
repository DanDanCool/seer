# pragma once

#include <string>
#include <fstream>
#include <vector>
#include <assert.h>
#include "VisionThread.h"

class BBoxWriter {
public:
	BBoxWriter(const std::string& fname)
	: file(fname, std::ios::binary | std::ios::out) {
	}

	~BBoxWriter() {
	}

	void Write(const std::vector<BBoxData>& data) {
		const uint32_t nullbytes = -1;
		const uint32_t size = data.size();
		file.write((char*)&nullbytes, sizeof(uint32_t));
		file.write((char*)&size, sizeof(uint32_t));
		for (auto& bbox : data) {
			file.write((char*)&bbox, sizeof(BBoxData));
		}

		file.flush();
	}

private:
	std::ofstream file;
};

class BBoxReader {
public:
	BBoxReader(const std::string& fname)
		: file(fname, std::ios::binary | std::ios::in) {
	}

	void Read(std::vector<BBoxData>& data) {
		uint32_t buf;
		file.read((char*)&buf, sizeof(uint32_t));
		assert(buf == -1);
		file.read((char*)&buf, sizeof(uint32_t));

		for (int i = 0; i < buf; i++) {
			BBoxData bbox;
			file.read((char*)&bbox, sizeof(float) * 4);
			data.push_back(bbox);
		}
	}

private:
	std::ifstream file;
};
