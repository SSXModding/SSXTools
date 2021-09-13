//
// Created by lily on 7/18/21.
//

#ifndef SSXTOOLS_CHUNKUNION_H
#define SSXTOOLS_CHUNKUNION_H

#include <ssxtools/shps/Structures.h>

namespace ssxtools::shps {

	namespace detail {
		/**
		 * Returns the combined size of however many objects you put into this template.
		 * \code
		 * constexpr auto AllTheSizes = CombinedSizeOf<int, std::string, unsigned long, short, double>();
		 * \endcode
		 */
		template<class... T>
		consteval std::size_t CombinedSizeOf() {
			return (...+sizeof(T));
		}

		/**
		 * Helper type.
		 */
		template<class... T>
		using UnionMemory = std::uint8_t[CombinedSizeOf<T...>()];

	}

	/**
	 * A tagged container for chunks,
	 * which automatically handles reading and accessing of the proper chunk data type(s).
	 */
	struct ChunkUnion {
		enum class GeneralizedType : std::uint8_t {
			Error,
			Image,
			Palette,
			FullName
		};

		~ChunkUnion();

		template<core::Stream Stream>
		inline bool Transform(Stream& stream) {
			SSXTOOLS_CATCH_ERROR(stream.template Other(header));

			// Populate the generalized type if reading.
			if constexpr(Stream::IsReadStream())
				GeneralType = GeneralizedTypeFromChunkType(header.type);

			// if not writing, we might need to add the alignment data.

			switch(GetGeneralType()) {
				case GeneralizedType::Image:
				case GeneralizedType::Palette:
					SSXTOOLS_CATCH_ERROR(TransformUnion<structures::ShapeHeaderChunk>(a.shape, stream));
					break;
				case GeneralizedType::FullName:
					SSXTOOLS_CATCH_ERROR(TransformUnion<structures::FullNameChunk>(b.fullName, stream));
					break;
			}
			return true;
		}

		// operator=& template

		structures::Chunk& GetHeader();
		const structures::Chunk& GetHeader() const;

		GeneralizedType GetGeneralType() const;

		/**
		 * Get a given fully defined chunk type. Useful for visiting this chunk, like:
		 * \code
		 * if(chunkUnion.GetGeneralType() == ChunkUnion::GeneralizedType::Image) {
		 * 	// This is a shape header, let's visit it!
		 * 	const ShapeHeaderChunk& shape = chunkUnion.GetChunk<ShapeHeaderChunk>();
		 * 	auto n = shape.width; // pull out some information
		 * }
		 * \endcode
		 */
		template<class T>
		inline T& GetChunk() {
			if constexpr(std::is_same_v<T, structures::ShapeHeaderChunk>) {
				assert(GetGeneralType() == GeneralizedType::Image || GetGeneralType() == GeneralizedType::Palette);
				return a.shape;
			} else if constexpr(std::is_same_v<T, structures::FullNameChunk>) {
				assert(GetGeneralType() == GeneralizedType::FullName);
				return b.fullName;
			}
		}

		template<class T>
		inline const T& GetChunk() const {
			if constexpr(std::is_same_v<T, structures::ShapeHeaderChunk>) {
				assert(GetGeneralType() == GeneralizedType::Image || GetGeneralType() == GeneralizedType::Palette);
				return a.shape;
			} else if constexpr(std::is_same_v<T, structures::FullNameChunk>) {
				assert(GetGeneralType() == GeneralizedType::FullName);
				return b.fullName;
			}
		}

	   private:
		structures::Chunk header;
		GeneralizedType GeneralType { GeneralizedType::Error };

		static GeneralizedType GeneralizedTypeFromChunkType(structures::Chunk::Type type);

		// Helper to shorten the transform switch.
		// Automatically initalizes the right member
		template<core::Stream Stream, class T>
		inline bool TransformUnion(T& union_member, Stream& stream) {
			// Placement-new (calling the constructor)
			// only if we need to.
			if constexpr(Stream::IsReadStream())
				new(&union_member) T();

			SSXTOOLS_CATCH_ERROR(stream.template Other(union_member));
		}

		// Tagged union. The generalized type member tags which member is active.
		// Probably a bit wasteful, but whatever.
		union {
			// the whole memory of the union
			detail::UnionMemory<structures::ShapeHeaderChunk, structures::FullNameChunk> memory;

			// accessor used when header.type is a image
			struct {
				structures::ShapeHeaderChunk shape;
			} a;

			// accessor used when header.type is the fullname chunk
			struct {
				structures::FullNameChunk fullName;
			} b;
		};
	};
} // namespace ssxtools::shps

#endif //SSXTOOLS_CHUNKUNION_H
