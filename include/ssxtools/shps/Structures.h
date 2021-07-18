//
// Created by lily on 7/13/21.
//

#ifndef SSXTOOLS_STRUCTURES_H
#define SSXTOOLS_STRUCTURES_H

#include <cstdint>
#include <cassert>
#include <string>

#include <ssxtools/core/Stream.h>
#include <ssxtools/core/FourCC.h>
#include <ssxtools/core/uint24.h>

namespace ssxtools::shps::structures {

	struct FileHeader {
		/**
		 * Valid SHPS file magic.
		 */
		constexpr static std::uint32_t FileMagic = core::FourCCValue("SHPS");

		std::uint32_t magic;
		std::uint32_t FileSize;
		std::uint32_t ShapeCount;
		std::uint32_t Creator;

		/**
		 * A entry in the shape Table Of Contents.
		 */
		struct TocEntry {
			char Name[4];
			std::uint32_t StartOffset;

			template<core::Stream Stream>
			inline bool Transform(Stream& stream) {
				SSXTOOLS_CATCH_ERROR(stream.template FixedString(Name));
				SSXTOOLS_CATCH_ERROR(stream.template Uint32<std::endian::little>(StartOffset));
				return true;
			}
		};

		template<core::Stream Stream>
		inline bool Transform(Stream& stream) {
			SSXTOOLS_CATCH_ERROR(stream.template Uint32<std::endian::little>(magic));
			SSXTOOLS_CATCH_ERROR(stream.template Uint32<std::endian::little>(FileSize));
			SSXTOOLS_CATCH_ERROR(stream.template Uint32<std::endian::little>(ShapeCount));
			SSXTOOLS_CATCH_ERROR(stream.template Uint32<std::endian::little>(Creator));
			return true;
		}
	};

	/**
	 * A chunk. Use ChunkUnion for polymorphic usage.
	 */
	struct Chunk {
		enum class Type : std::uint8_t {
			Image4Bpp = 0x1,
			Image8Bpp = 0x2,
			Image32Bpp = 0x5,
			Palette = 0x21,
			FullName = 0x70,
			Hotspot = 0x71
		} type;

		/**
		 * Offset to the next chunk.
		 * 0 means this is the last chunk
		 */
		core::uint24le next_chunk;

		template<core::Stream Stream>
		inline bool Transform(Stream& stream) {
			SSXTOOLS_CATCH_ERROR(stream.Byte(reinterpret_cast<std::uint8_t&>(type)));
			SSXTOOLS_CATCH_ERROR(stream.template Other(next_chunk));
			return true;
		}
	};

	/**
	 * Shape header chunk.
	 * For images which have a palette, this chunk is also used for that. Why? I don't know. Ask EA.
	 */
	struct ShapeHeaderChunk {
		std::uint16_t width;
		std::uint16_t height;
		// These bitfields aren't important yet.
		std::uint32_t unk_bitfield_1;
		std::uint32_t unk_bitfield_2;

		// Maybe have the data here? Idk

		template<core::Stream Stream>
		inline bool Transform(Stream& stream) {
			SSXTOOLS_CATCH_ERROR(stream.template Uint16<std::endian::little>(width));
			SSXTOOLS_CATCH_ERROR(stream.template Uint16<std::endian::little>(height));
			SSXTOOLS_CATCH_ERROR(stream.template Uint32<std::endian::little>(unk_bitfield_1));
			SSXTOOLS_CATCH_ERROR(stream.template Uint32<std::endian::little>(unk_bitfield_2));
			return true;
		}
	};

	/**
	 * Full name chunk. This provides the full name of a shape
	 * if it's wanted.
	 */
	struct FullNameChunk {
		std::string name;

		template<core::Stream Stream>
		inline bool Transform(Stream& stream) {
			SSXTOOLS_CATCH_ERROR(stream.String(name));
			return true;
		}
	};

	// static_assert's to make sure all this is lined up with the ssh docs
	static_assert(sizeof(FileHeader) == 16);
	static_assert(sizeof(FileHeader::TocEntry) == 8);
	static_assert(sizeof(Chunk) == 4);
	static_assert(sizeof(ShapeHeaderChunk) == 12);

} // namespace ssxtools::shps::structures

#endif //SSXTOOLS_STRUCTURES_H
