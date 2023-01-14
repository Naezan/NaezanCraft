#pragma once

#include "serializer.h"

struct FileMemory
{
	FileMemory();

	unsigned char* rawdata;
	size_t datasize;
	size_t readoffset;

	std::vector<unsigned char> data;
	zpp::serializer::memory_input_archive in;
	zpp::serializer::memory_output_archive out;

	void allocatedata(size_t _size)
	{
		rawdata = new unsigned char[_size];
		datasize = _size;
	}

	template<typename T>
	void write(const T writeData)
	{
		out(writeData);

		rawdata = out.data();
		datasize = out.offset();
	}

	template<typename T>
	void read(T* writeData)
	{
		//rawdata + offset부터 sizeof(writeData)크기만큼의 데이터를 불러온다
		memcpy_s(writeData, sizeof(T), rawdata + readoffset, sizeof(T));
		readoffset += sizeof(T);
	}

	void reset(bool isallocate)
	{
		//TO DO reset out in?
		//out.reset();
		if (isallocate)
			delete[] rawdata;
		rawdata = nullptr;
		datasize = 0;
	}
};