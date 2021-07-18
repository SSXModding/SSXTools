//
// Created by lily on 7/18/21.
//

#include <ssxtools/shps/ChunkUnion.h>

namespace ssxtools::shps {

	ChunkUnion::~ChunkUnion() {
		// Destruct the currently active tagged object
		// to avoid memory leaking (if there were say a vector or string.)
		switch(GetGeneralType()) {
			case GeneralizedType::Image:
			case GeneralizedType::Palette:
				a.shape.~ShapeHeaderChunk();
				break;
			case GeneralizedType::FullName:
				b.fullName.~FullNameChunk();
				break;
		}
	}

	structures::Chunk& ChunkUnion::GetHeader() {
		return header;
	}

	const structures::Chunk& ChunkUnion::GetHeader() const {
		return header;
	}

	ChunkUnion::GeneralizedType ChunkUnion::GetGeneralType() const {
		return GeneralType;
	}

	ChunkUnion::GeneralizedType ChunkUnion::GeneralizedTypeFromChunkType(structures::Chunk::Type type) {
		// that's a google code name right there...
		// meh
		switch(type) {
			case structures::Chunk::Type::Image4Bpp:
			case structures::Chunk::Type::Image8Bpp:
			case structures::Chunk::Type::Image32Bpp:
				return GeneralizedType::Image;
			case structures::Chunk::Type::Palette: // Ask EA, not me
				return GeneralizedType::Palette;
			case structures::Chunk::Type::FullName:
				return GeneralizedType::FullName;
		}
		return GeneralizedType::Error;
	}
}