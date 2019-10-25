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

// Print a char array's contents explicitly until the end of the array.
#define PrintFixedArray(a, size, os)	{									\
											std::stringstream ss;			\
											for(auto i = 0u; i < size; ++i) \
												ss << a[i];					\
											os << ss.str();					\
											ss.clear();						\
										}

// strncmp() alias for readability
#define SizedCmp(a,b) !strncmp(a, b, sizeof(a))

// strncmp() alias for readability that accepts a amount to take away from sizeof
#define SizedCmpOff(a,b,off) !strncmp(a, b, sizeof(a)-off)