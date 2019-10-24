#pragma once
#include <cstdint>

// Version of a EAGL texture container
enum ShpsVersion : std::uint32_t {
	Version1 = 1
};

// Header of SHPS image
struct ShpsHeader {
	// File magic (SHPS)
	char Magic[4];

	// Size of entire file in bytes
	std::uint32_t FileLength;

	// Version of EAGL texture format this file is using
	std::uint32_t FileVersion;

	// Magic of image
	char FileMagic[4];

	// Internal file code
	char FileName[4];

	// Size of header
	std::uint32_t HeaderSize;

	// Image group name
	char GroupName[4];

	// ERTS header magic (ERTS)
	char ErtsMagic[4];

	// Garbage data
	std::uint32_t Garbage;

	// Image width
	std::uint16_t ImageWidth;

	// Image height
	std::uint16_t ImageHeight;

	// More garbage data after image height information
	std::uint64_t NullData;
};
