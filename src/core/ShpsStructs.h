#pragma once
#include <cstdint>
#include "Util.h"

namespace eagle {
	namespace core {

		// GIMEX versions.
		// We'll need these for some Gimex version specific stuff

		// Gimex version for SSX (2000). Doesn't seem to put a real version here
		constexpr static char GimexVersion_SSX[] = "GIMX";

		// Gimex version for SSX Tricky (2001).
		constexpr static char GimexVersion_SSXT[] = "G278";

		/*
		 * Header of SHP shape files. 
		 */
		struct ShpsFileHeader {
			// File magic (SHPS)
			char Magic[4];

			// Size of entire file in bytes
			uint32 FileLength;

			// Texture file count
			uint32 FileTextureCount;

			// A code describing the Gimex version that
			// this shape file
			char GimexVersion[4];
		};

		/**
		 * Shape file TOC entry
		 */
		struct ShpsTocEntry {
			/**
			 * Name of the image.
			 */
			// NOTE: SSX 3 has a Gimex extension that sometimes puts the full
			// name of the texture after it.
			char Name[4];

			/**
			 * Start offset.
			 */
			uint32 StartOffset;
		};

		// This is taken from the first uint16 in ShpsImageHeader.
		// These seem to stay the same for certain image types.
		enum ShpsImageType : uint32 {
			Unknown,

			// 4bpp image
			// These may be the stupid compression thing
			// I saw in GX...
			Lut128 = 0x1001,

			// 256 color image
			Lut256 = 0x1002,

			// 32bpp, no LUT
			NonLut32Bpp = 0x0005
		};

		/**
		 * Shape file per-image header
		 */
		struct ShpsImageHeader {
			// This is probably a byte
			uint16 format;

			uint16 unknown;

			uint16 width;

			uint16 height;

			uint32 unknown2;

			uint32 unknown3;

			// image data starts after this
		};

		/**
		 * Shape file LUT header
		 */
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
		union ShpsRgba {
			/**
			 * total value
			 */
			uint32 total;

			/**
			 * Struct accessor for accessing individual components of the color
			 */
			struct {
				byte b;

				byte g;

				byte r;

				byte a;
			};
		};

		/**
		 * EAGLE-specific extension stuff
		 */
		struct ShpsImage : public ShpsImageHeader {
			/**
			 * The TOC entry this image is under.
			 */
			ShpsTocEntry toc_entry;

			// this is a huge hack, remove it
			/**
			 * Image index
			 */
			int index;

			/**
			 * The image data.
			 *
			 * Under LUT256 and LUT128, this will contain indexes suitable for the palette array.
			 */
			std::vector<byte> data;

			/**
			 * Palette used if this image is palettized.
			 */
			std::vector<ShpsRgba> palette;
		};

	} // namespace core
} // namespace eagle
