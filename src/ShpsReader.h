#pragma once

#include <iostream>
#include <vector>
#include <exception>
#include <fstream>
#include "ShpsStructs.h"

namespace eagle {
namespace core {

	/**
	 * Reads SHPS (.ssh) files to data structures that can be serialized
	 * to other formats easily.
	 */
	struct ShpsReader {

		ShpsReader(std::istream& stream, std::string& filename) 
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
		 * Get the file header.
		 */
		EAGLE_GETTER(ShpsFileHeader&, GetHeader) {
			return header;
		}

		/**
		 * Get the image TOC.
		 */
		EAGLE_GETTER(std::vector<ShpsTocEntry>&, GetTOC) {
			return toc;
		}

		/**
		 * Get image count.
		 */
		EAGLE_GETTER(std::size_t, GetImageCount) {
			return images.size();
		}
		
		/**
		 * Get all images.
		 */
		EAGLE_GETTER(std::vector<ShpsImage>&, GetImages) {
			return images;
		}
		
		/**
		 * Get the SSH filename.
		 */
		EAGLE_GETTER(std::string&, GetFileName) {
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