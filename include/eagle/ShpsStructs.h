#pragma once
#include <cstdint>
#include <eagle/Core.h>
#include <eagle/uint24.h>

namespace eagle::core::shps {

	// GIMEX versions.
	// We'll need these for some Gimex version specific stuff

	// Gimex version for SSX (2000). Doesn't seem to put a real version here
	constexpr static uint32 GimexVersion_SSX = MakeFourCCValue("GIMX");

	// Gimex version for SSX Tricky (2001).
	constexpr static uint32 GimexVersion_SSXT = MakeFourCCValue("G278");

	// Gimex version for SSX 3.
	constexpr static uint32 GimexVersion_SSX3 = MakeFourCCValue("G357");
	constexpr static uint32 GimexVersion_SSX3Alt = MakeFourCCValue("G355"); // 091103 build

	/*
	 * Header of Gimex shape files.
	 */
	struct FileHeader {
		// File magic (SHPS)
		char Magic[4];

		/**
		 * Size of the entire shape file in bytes.
		 */
		uint32 FileLength;

		/**
		 * The count of textures in this shape file
		 */
		uint32 FileTextureCount;

		/**
		 * A 4-byte code of the version of Gimex
		 * used to create the shape file.
		 */
		uint32 creator;

		// TODO: use the array hack that gimex seems to already use?
	};

	/**
	 * Shape file TOC entry
	 */
	struct TocEntry {
		/**
		 * Name of the image.
		 */
		// NOTE: SSX 3 has a Gimex extension that sometimes puts the full
		// name of the texture after the clut.
		char Name[4];

		/**
		 * Start offset.
		 */
		uint32 StartOffset;
	};

	/**
	 * Shape image format.
	 */
	enum class ShapeImageType : byte {
		Unknown,

		// 4bpp image
		Lut128 = 0x1,

		// 256 color image
		Lut256 = 0x02,

		// 32bpp BGRA, no LUT
		NonLut32Bpp = 0x05
	};


	// this is probably a byte like ImageType
	enum EncodingType : uint32 {
		None,
		Interleaved = 0x00200000,
		Unknown
	};

	/**
	 * Shape file per-shape header.
	 */
	struct ImageHeader {
		/**
		 * Shape format.
		 */
		ShapeImageType format;

		/**
		 * Offset to the start of the CLUT header.
		 */
		uint24le clut_offset;

		uint16 width;

		uint16 height;

		// TODO: these are bit fields too(TM)
		uint32 unknown2;

		uint32 unknown3;

		// image data starts after this
	};

	/**
	 * Shape file LUT header
	 */
	struct PaletteHeader {
		// This is mostly used as a marker.
		// TODO
		char unknown[4];

		// Is this even used?
		// Not sure
		uint16 colorCount;

		uint16 unknown2;

		uint32 unknown3;

		uint32 unknown4;

		// palette data as a array of bgra8888
	};

	/**
	 * 4-byte RGBA color, in BGRA order
	 */
	union Bgra8888 {
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
	 * EAGLE-specific extension structure to hold image data as well
	 */
	struct Image : public shps::ImageHeader {
		/**
		 * The TOC entry this image is under.
		 */
		shps::TocEntry toc_entry;

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
		std::vector<shps::Bgra8888> palette;

		// TODO: std::optional<>?
		/**
		 * Full image name.
		 */
		std::string FullName;

		// TODO: extension functions
	};

} // namespace eagle::core::shps

namespace eagle::core {

	/**
	 * Specialization of EnumToString for ShapeImageType
	 */
	template<>
	constexpr const char* EnumToString<shps::ShapeImageType>(shps::ShapeImageType type) {
		switch(type) {
			case shps::ShapeImageType::Lut128:
				return "4bpp";
				break;
			case shps::ShapeImageType::Lut256:
				return "8bpp";
				break;
			case shps::ShapeImageType::NonLut32Bpp:
				return "32bpp";
				break;
			default:
				return "Unknown";
				break;
		}
	}

}