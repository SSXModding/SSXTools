#pragma once
#include "Util.h"
#include "ShpsStructs.h"
#include <string>
#include <filesystem>
#include <functional>

namespace eagle {
	namespace core {

		enum WriterProgressType {
			Info,
			Error
		};

		/**
		 * The SHPS writer component of EAGLe.
		 */
		struct ShpsWriter {
			/**
			 * Progress function signature.
			 */
			typedef std::function<void(std::string, WriterProgressType)> ProgressFunction;

			/**
			 * Set the progress function for the writer.
			 *
			 * \param[in] new_func The new function to use.
			 */
			void SetProgressFunction(ProgressFunction new_func);

			/**
			 * Builds a normal 32bpp RGBA image from a SHPS image.
			 * Returns false on failure/invalid format, true otherwise.
			 *
			 * \param[in] imageBuffer Image buffer to write to.
			 * \param[in] image SHPS image to write
			 */
			bool BuildImageBuffer(std::vector<byte>& imageBuffer, ShpsImage& image);

			/**
			 * Write the provided SHPS image to a PNG file.
			 *
			 * Returns false on failure (progress function will output a possible reason),
			 * or true on successful image write..
			 *
			 * \param[in] image Image to write.
			 * \param[in] input_path The input path. Must contain a filename.
			 * \param[in] output_path The output path.
			 */
			bool WritePNG(ShpsImage& image, const std::filesystem::path& input_path, const std::filesystem::path& output_path);
		};

	} // namespace core
} // namespace eagle