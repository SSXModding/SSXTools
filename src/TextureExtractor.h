#pragma once

#include <iostream>
#include <vector>
#include <exception>
#include <fstream>
#include "ShpsStructs.h"

namespace eagle {
namespace core {

	// Extracts texture from STL stream.
	struct TextureExtractor {
		TextureExtractor(std::istream& stream, 
			std::string& filename) 
			: stream(stream),
			  filename(filename) {
		
		}
		
		/**
		 * Read the SHPS header.
		 */
		void ReadHeader();

		/**
		 * Read the Table Of Contents (TOC),
		 * which contains the filecode of the file
		 * and the start offset of the image.
		 */
		void ReadTOC();

		/**
		 * Check if a provided header is valid.
		 *
		 * \param[in] header File header to check.
		 */
		bool CheckValidHeader(const ShpsFileHeader& header);
		
		/**
		 * Read a image at the provided TOC index.
		 *
		 * \param[in] index Index to read.
		 */
		ShpsImage ReadImage(int imageIndex);

		/**
		 * Read a image at the provided TOC index.
		 *
		 * \param[in] Index of image to write.
		 */
		void WriteImage(int index);
		
		/**
		 * Get the file header.
		 */
		EAGLE_GETTER(ShpsFileHeader, GetHeader) {
			return header;
		}

		/**
		 * Get the image TOC.
		 */
		EAGLE_GETTER(std::vector<ShpsTocEntry>, GetTOC) {
			return toc;
		}

		/**
		 * Get image count.
		 */
		EAGLE_GETTER(int, GetImageCount) {
			return images.size();
		}
		
		/**
		 * Get all images.
		 */
		EAGLE_GETTER(std::vector<ShpsImage>, GetImages) {
			return images;
		}
		
		/**
		 * Get the SSH filename.
		 */
		EAGLE_GETTER(std::string, GetFileName) {
			return filename;
		}

	private:

		std::istream& stream;
		std::string filename;

		// Image stuff
		ShpsFileHeader header{};
		std::vector<ShpsTocEntry> toc;
		std::vector<ShpsImage> images;
	};

}
}