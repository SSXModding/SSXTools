#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <ssxtools/shps/ShpsStructs.h>
#include <modeco/BinaryReader.h>

namespace ssxtools::shps {

	/**
	 * Reads SHPS (.ssh) texture banks to data structures that can be serialized
	 * to other formats easily.
	 */
	struct ShpsReader {
		ShpsReader(std::istream& stream, std::string& filename)
			: reader(stream),
			  filename(filename) {
		}

		/**
		 * Read the SHPS header.
		 */
		bool ReadHeader();

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
		bool CheckValidHeader(const FileHeader& header);

		/**
		 * Read a image at the provided TOC index.
		 *
		 * \param[in] index Index to read.
		 */
		Image ReadImage(int imageIndex);

		/**
		 * Get the file header.
		 */
		inline FileHeader& GetHeader() {
			return header;
		}

		/**
		 * Get the image TOC.
		 */
		inline std::vector<TocEntry>& GetTOC() {
			return toc;
		}

		/**
		 * Get image count.
		 */
		std::size_t GetImageCount() {
			return images.size();
		}

		/**
		 * Get all images.
		 */
		std::vector<Image>& GetImages() {
			return images;
		}

		/**
		 * Get the SSH filename.
		 */
		std::string& GetFileName() {
			return filename;
		}

	   private:
		mco::BinaryReader reader;

		std::string filename;

		// Image stuff
		shps::FileHeader header {};
		std::vector<TocEntry> toc;
		std::vector<Image> images;
	};

} // namespace ssxtools::shps