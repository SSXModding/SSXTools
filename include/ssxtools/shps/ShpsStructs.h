#pragma once
#include <cstdint>
#include <ssxtools/Core.h>
#include <ssxtools/core/uint24.h>

namespace ssxtools::shps {

	// GIMEX versions.
	// We'll need these for some Gimex version specific stuff

	// Gimex version for SSX (2000). Doesn't seem to put a real version here
	constexpr static core::uint32 GimexVersion_SSX = core::MakeFourCCValue("GIMX");

	// Gimex version for SSX Tricky (2001).
	constexpr static core::uint32 GimexVersion_SSXT = core::MakeFourCCValue("G278");

	// Gimex version for SSX 3.
	constexpr static core::uint32 GimexVersion_SSX3 = core::MakeFourCCValue("G357");
	constexpr static core::uint32 GimexVersion_SSX3Alt = core::MakeFourCCValue("G355"); // 091103 build

	/*
	 * Header of Gimex shape files.
	 */
	struct FileHeader {
		// File magic (SHPS)
		core::uint32 Magic;

		/**
		 * Size of the entire shape file in bytes.
		 */
		core::uint32 FileLength;

		/**
		 * The count of textures in this shape file
		 */
		core::uint32 FileTextureCount;

		/**
		 * A 4-byte code of the version of Gimex
		 * used to create the shape file.
		 */
		core::uint32 creator;

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
		core::uint32 StartOffset;
	};

	/**
	 * Shape image format.
	 */
	enum class ShapeImageType : core::byte {
		Unknown,

		// 4bpp image
		Lut128 = 0x1,

		// 256 color image
		Lut256 = 0x02,

		// 32bpp BGRA, no LUT
		NonLut32Bpp = 0x05
	};

	// this is probably a byte like ImageType
	enum EncodingType : core::uint32 {
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
		core::uint24le clut_offset;

		core::uint16 width;

		core::uint16 height;

		// TODO: these are bit fields too(TM)
		core::uint32 unknown2;

		core::uint32 unknown3;

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
		core::uint16 colorCount;

		core::uint16 unknown2;

		core::uint32 unknown3;

		core::uint32 unknown4;

		// palette data as a array of bgra8888
	};

	/**
	 * 4-byte RGBA color, in BGRA order
	 */
	union Bgra8888 {
		/**
		 * total value
		 */
		core::uint32 total;

		/**
		 * Struct accessor for accessing individual components of the color
		 */
		struct {
			core::byte b;

			core::byte g;

			core::byte r;

			core::byte a;
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
		std::vector<core::byte> data;

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

} // namespace ssxtools::shps::shps

namespace ssxtools::core {

	/**
	 * Specialization of EnumToString for ShapeImageType
	 */
	template<>
	constexpr const char* EnumToString<shps::ShapeImageType>(shps::ShapeImageType type) {
		switch(type) {
			case shps::ShapeImageType::Lut128:
				return "4bpp";

			case shps::ShapeImageType::Lut256:
				return "8bpp";

			case shps::ShapeImageType::NonLut32Bpp:
				return "32bpp";

			default:
				return "Unknown";
		}
	}

} // namespace ssxtools::shps