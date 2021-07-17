//
// Created by lily on 7/13/21.
//

#ifndef SSXTOOLS_STRUCTURES_H
#define SSXTOOLS_STRUCTURES_H

#include <cstdint>
#include <cassert>

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
			SSXTOOLS_CATCH_ERROR(stream.template TransformOther(next_chunk));
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

	/**
	 * A tagged container for chunks,
	 * which automatically handles reading and accessing of the proper chunk type.
	 * TODO: Should this be here? Seems a bit higher level than the structures here.
	 */
	struct ChunkUnion {
		template<core::Stream Stream>
		inline bool Transform(Stream& stream) {
			SSXTOOLS_CATCH_ERROR(stream.template TransformOther(header));
			// if not writing, we might need to add the alignment data.
			switch(header.type) {
				case Chunk::Type::Image4Bpp:
				case Chunk::Type::Image8Bpp:
				case Chunk::Type::Image32Bpp:
				case Chunk::Type::Palette: // Ask EA, not me
					SSXTOOLS_CATCH_ERROR(TransformUnion<ShapeHeaderChunk>(shape, stream));

				case Chunk::Type::FullName:
					SSXTOOLS_CATCH_ERROR(TransformUnion<FullNameChunk>(name, stream));
			}
			return true;
		}

		inline Chunk& GetHeader() {
			return header;
		}

		inline const Chunk& GetHeader() const {
			return header;
		}

		/**
		 * Get a given fully defined chunk type
		 */
		template<class T>
		inline T& GetChunk() {
			if constexpr(std::is_same_v<T, ShapeHeaderChunk>) {
				switch(header.type) {
					case Chunk::Type::Image4Bpp:
					case Chunk::Type::Image8Bpp:
					case Chunk::Type::Image32Bpp:
					case Chunk::Type::Palette:
						return shape;
				}
				// This assert shouldn't be reached
				assert(false);
			}

			if constexpr(std::is_same_v<T, FullNameChunk>) {
				assert(header.type == Chunk::Type::FullName);
				return name;
			}
		}

	   private:
		Chunk header;

		// helper to shorten the transform switch
		template<core::Stream Stream, class T>
		inline bool TransformUnion(T& t, Stream& stream) {
			// This placement new should only be done for reading.
			if constexpr(Stream::IsReadStream())
				new(&t) T();
			SSXTOOLS_CATCH_ERROR(stream.template TransformOther(t));
		}

		union {
			// the whole memory of the union
			std::uint8_t memory[sizeof(ShapeHeaderChunk) + sizeof(FullNameChunk)];

			// accessor used when header.type is a image
			struct {
				ShapeHeaderChunk shape;
			};

			// accessor used when header.type is the fullname chunk
			struct {
				FullNameChunk name;
			};
		};
	};

	// static_assert's to make sure all this is lined up with the ssh docs
	static_assert(sizeof(FileHeader) == 16);
	static_assert(sizeof(FileHeader::TocEntry) == 8);
	static_assert(sizeof(Chunk) == 4);
	static_assert(sizeof(ShapeHeaderChunk) == 12);

} // namespace ssxtools::shps::structures

#endif //SSXTOOLS_STRUCTURES_H
