// This header file is deprecated, the replacement is shps/Structures.h
#pragma once
#include <cstdint>
#include <ssxtools/Core.h>
#include <ssxtools/core/FourCC.h>
#include <ssxtools/core/uint24.h>

namespace ssxtools::shps {

	// GIMEX versions.
	// We'll need these for some Gimex version specific stuff

	// Gimex version for SSX (2000). Doesn't seem to put a real version here
	constexpr static std::uint32_t GimexVersion_SSX = core::FourCCValue("GIMX");

	// Gimex version for SSX Tricky (2001).
	constexpr static std::uint32_t GimexVersion_SSXT = core::FourCCValue("G278");

	// Gimex version for SSX 3.
	constexpr static std::uint32_t GimexVersion_SSX3 = core::FourCCValue("G357");
	constexpr static std::uint32_t GimexVersion_SSX3Alt = core::FourCCValue("G355"); // 091103 build

	/*
	 * Header of Gimex shape files.
	 */
	struct FileHeader {
		// File magic (SHPS)
		std::uint32_t Magic;

		/**
		 * Size of the entire shape file in bytes.
		 */
		std::uint32_t FileLength;

		/**
		 * The count of textures in this shape file
		 */
		std::uint32_t FileTextureCount;

		/**
		 * A 4-byte code of the version of Gimex
		 * used to create the shape file.
		 */
		std::uint32_t creator;

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
		std::uint32_t StartOffset;
	};

	/**
	 * Shape image format.
	 */
	enum class ShapeImageType : std::uint8_t {
		Unknown,

		// 4bpp image
		Lut128 = 0x1,

		// 256 color image
		Lut256 = 0x02,

		// 32bpp BGRA, no LUT
		NonLut32Bpp = 0x05
	};

	// this is probably a byte like ImageType
	enum EncodingType :  std::uint32_t {
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

		std::uint16_t width;

		std::uint16_t height;

		// TODO: these are bit fields too(TM)
		std::uint32_t unknown2;

		std::uint32_t unknown3;

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
		std::uint16_t colorCount;

		std::uint16_t unknown2;

		std::uint32_t unknown3;

		std::uint32_t unknown4;

		// palette data as a array of bgra8888
	};

	/**
	 * 4-byte RGBA color, in BGRA order
	 */
	union Bgra8888 {
		/**
		 * total value
		 */
		std::uint32_t total;

		/**
		 * Struct accessor for accessing individual components of the color
		 */
		struct {
			std::uint8_t b;

			std::uint8_t g;

			std::uint8_t r;

			std::uint8_t a;
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
		std::vector<std::uint8_t> data;

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