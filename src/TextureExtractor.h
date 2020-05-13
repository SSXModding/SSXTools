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

		void ReadHeader();

		void ReadTOC();

		bool CheckValidHeader(const ShpsFileHeader& header);
		
		ShpsImage ReadImage(int imageIndex);

		/**
		 * Write an image.
		 */
		void WriteImage(int index);
		
		/**
		 * Get the file header
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