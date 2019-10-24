#pragma once
#include <fstream>

// Reads a data structure from a open stream
template<class Struct> 
inline void ReadStruct(std::istream& stream, Struct& st) {
	if (!stream)
		throw std::invalid_argument("Bad stream passed to ReadStruct<Struct>()");

	char buf[sizeof(Struct)];
	stream.read(buf, sizeof(Struct));
	memmove(&st, &buf[0], sizeof(Struct));
}

#define PrintFixedArray(a, size, os) for(auto i = 0u; i < size; ++i) os << a[i];