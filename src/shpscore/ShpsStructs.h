#pragma once
#include <cstdint>
#include "Util.h"

namespace eagle {
namespace core {

	// File type for SSX (2000).
	constexpr static char FileType_SSX1[] = "GIMX";

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

		// 32bpp, no LUT
		NonLut32Bpp = 0x0005
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

	/**
	 * 4-byte RGBA color.
	 */
	struct ShpsRgba {
		// Stored as BGRA
		union {
			uint32 total;

			struct {
				byte b;

				byte g;

				byte r;

				/**
				 * The alpha component of the color.
				 *
				 * The alpha component in SHPS images
				 * is stored as a single byte with the range
				 * of 0x00 (0) to 0x80 (128).
				 *
				 * This effectively means that there is only half a byte
				 * of alpha precision (as 1 will become 2, 2 will become 4, so on).
				 *
				 */
				byte a;
			};
		} color;
	};

	struct ShpsImage : public ShpsImageHeader {
		/**
		 * The TOC entry this image is under.
		 */
		ShpsTocEntry toc_entry;

		/**
		 * Image index
		 */
		int index;

		/**
		 * The image data.
		 *
		 * Under LUT256, this will contain indexes suitable for the palette array.
		 */
		std::vector<byte> data;

		/**
		 * Palette used if this image is palettized.
		 */
		std::vector<ShpsRgba> palette;
	};


#undef unknown

}
}
