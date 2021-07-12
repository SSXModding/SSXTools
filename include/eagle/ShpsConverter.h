#pragma once
#include "Core.h"
#include "ShpsStructs.h"
#include <string>
#include <filesystem>
#include <functional>

#include <modeco/Logger.h>
#ifdef INQT
	#undef error
	#undef verbose
#endif

namespace eagle::core {

	/**
	 * The SHPS->PNG writer component of EAGLe.
	 */
	struct ShpsConverter {
		/**
		 * How many channels are in the final image buffer.
		 */
		constexpr static byte ChannelCount = 4;

		/**
		 * Builds a normal 32bpp RGBA image from a SHPS image.
		 * Returns false on failure/invalid format, true otherwise.
		 *
		 * \param[in] imageBuffer Image buffer to write to.
		 * \param[in] image SHPS image to write
		 */
		bool BuildImageBuffer(std::vector<byte>& imageBuffer, shps::Image& image, bool ssxHack = false);

		/**
		 * Write the provided SHPS image to a PNG file.
		 * This method will probably be removed soon as it's only purpose was basically
		 * for testing.
		 *
		 * Returns false on failure (progress function will output a possible reason),
		 * or true on successful image write..
		 *
		 * \param[in] image Image to write.
		 * \param[in] input_path The input path. Must contain a filename.
		 * \param[in] output_path The output path.
		 */
		bool WritePNG(shps::Image& image, const std::filesystem::path& input_path, const std::filesystem::path& output_path);

	   private:
		mco::Logger logger = mco::Logger::CreateLogger("SHPSConverter");
	};

} // namespace eagle::core