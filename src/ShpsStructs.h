#pragma once
#include <cstdint>
#include "Util.h"

namespace eagle {
namespace core {

// File type for SSX (2000).
constexpr static char FileType_SSX1[] = "GIMX";


// Mark unknown data with a specific size
#define unknown(suffix, size) char garbage##suffix[size];

	// Header of SHPS image
	struct ShpsFileHeader {
		// File magic (SHPS)
		char Magic[4];

		// Size of entire file in bytes
		uint32 FileLength;

		// Texture file count
		uint32 FileTextureCount;

		char FileType[4];
	};

	struct ShpsTocEntry {
		// Name of the image
		char Name[4];

		// Start offset of the image
		uint32 StartOffset;
	};


	// This is taken from the first uint16 in ShpsImageHeader.
	// These seem to stay the same for certain image types.
	enum ShpsImageType : uint32 {
		Unknown,

		// 256 color image
		Lut256 = 0x1002,

		// 256 color image
		//Lut256Unknown = TODO,

		// 32bpp, no LUT
		NonLut32Bpp = 0x1005
	};

	

	struct ShpsImageHeader {
		uint16 format;

		uint16 unknown;

		uint16 width;

		uint16 height;
		
		uint32 unknown2;

		uint32 unknown3;

		// image data starts after this
	};



	struct ShpsPaletteHeader {
		// This is mostly used as a marker.
		char unknown[4];

		// Is this even used?
		// Not sure
		uint16 colorCount;

		uint16 unknown2;

		uint32 unknown3;

		uint32 unknown4;

		// palette data
	};

	struct Shps8bppRgba {
		// Stored as BGRA
		union {
			uint32 total;

			struct {
				byte b;

				byte g;

				byte r;

				// range of A is 0x00 - 0x80
				// 0x00 means fully transparent
				// 0x80 means fully opaque
				byte a;
			};
		} color;
	};

	struct ShpsImage : public ShpsImageHeader {


		std::vector<byte> data;

		// Used if image is LUT256.
		std::vector<Shps8bppRgba> palette;
	};


#undef unknown

}
}
