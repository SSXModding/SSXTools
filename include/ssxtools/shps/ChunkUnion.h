//
// Created by lily on 7/18/21.
//

#ifndef SSXTOOLS_CHUNKUNION_H
#define SSXTOOLS_CHUNKUNION_H

#include <ssxtools/shps/Structures.h>

namespace ssxtools::shps {

	/**
	 * A tagged container for chunks,
	 * which automatically handles reading and accessing of the proper chunk data type(s).
	 * TODO: Should this be here? Seems a bit higher level than the structures here.
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
			if(Stream::IsReadStream())
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

		structures::Chunk& GetHeader();
		const structures::Chunk& GetHeader() const;

		GeneralizedType GetGeneralType() const;

		/**
		 * Get a given fully defined chunk type. Useful for visiting, like:
		 * \code
		 * if(chunkUnion.GetGeneralType() == ChunkUnion::GeneralizedType::Image) {
		 * 	// This is a shape header, let's visit it!
		 * 	ShapeHeaderChunk& shape = chunkUnion.GetChunk<ShapeHeaderChunk>();
		 * 	// now we can visit the shape header!
		 * }
		 * \endcode
		 *
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

	   private:
		structures::Chunk header;
		GeneralizedType GeneralType { GeneralizedType::Error };

		static GeneralizedType GeneralizedTypeFromChunkType(structures::Chunk::Type type);

		// Helper to shorten the transform switch.
		// Automatically
		template<core::Stream Stream, class T>
		inline bool TransformUnion(T& t, Stream& stream) {
			// This placement new should only be done for reading.
			if constexpr(Stream::IsReadStream())
				new(&t) T();
			SSXTOOLS_CATCH_ERROR(stream.template Other(t));
		}

		// Tagged union. The generalized type member tags which member is active.
		// Probably a bit wasteful, but whatever.
		union {
			// the whole memory of the union
			std::uint8_t memory[sizeof(structures::ShapeHeaderChunk) + sizeof(structures::FullNameChunk)];

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
