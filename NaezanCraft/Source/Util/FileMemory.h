#pragma once

#include "serializer.h"

struct FileMemory
{
	FileMemory();

	unsigned char* rawdata;
	size_t datasize;

	std::vector<unsigned char> data;
	zpp::serializer::memory_input_archive in;
	zpp::serializer::memory_output_archive out;

	template<typename T>
	void write(const T writeData)
	{
		out(writeData);

		rawdata = out.data(); 
		datasize = out.offset();
	}

	template<typename T>
	void read(T& writeData)
	{
		in(writeData);
	}

	void reset()
	{
		//TO DO reset out in?
		//out.reset();

		rawdata = nullptr;
		datasize = 0;
	}
};