#pragma once

#include <iostream>
#include <vector>
#include <exception>
#include <fstream>
#include "ShpsStructs.h"

namespace eagle {
	namespace core {

		// TODO: Use an impl pattern so that we can change stuff
		// without making dependent projects have to change every five minutes

		/**
		 * Reads SHPS (.ssh) texture banks to data structures that can be serialized
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
			inline ShpsFileHeader& GetHeader() {
				return header;
			}

			/**
			 * Get the image TOC.
			 */
			inline std::vector<ShpsTocEntry>& GetTOC() {
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
			std::vector<ShpsImage>& GetImages() {
				return images;
			}

			/**
			 * Get the SSH filename.
			 */
			std::string& GetFileName() {
				return filename;
			}

		   private:
			std::istream& stream;
			std::string filename;

			// Image stuff
			ShpsFileHeader header {};
			std::vector<ShpsTocEntry> toc;
			std::vector<ShpsImage> images;
		};

	} // namespace core
} // namespace eagle