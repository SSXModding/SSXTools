#pragma once
#include <fstream>

// Reads a data structure from a open stream
template<class Struct> 
inline void ReadStruct(std::istream& stream, Struct& st) {
	if (!stream)
		throw std::invalid_argument("stream passed was bad");

	char buf[sizeof(Struct)];
	stream.read(buf, sizeof(Struct));
	memmove(&st, &buf[0], sizeof(Struct));
}

// print a fixed char array
// (major bodge)
inline void PrintFixedArray(const char* array, const size_t size, std::ostream& s) {
	for (size_t i = 0; i < size; ++i)
			s << array[i];
}