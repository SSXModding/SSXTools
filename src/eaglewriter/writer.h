#pragma once
#include "Util.h"
#include "ShpsStructs.h"
#include <string>
#include <filesystem>
#include <functional>

namespace eagle {

/**
 * Namespace for the PNG writer component of EAGLe.
 */
namespace writer {

	enum ProgressType {
		Info,
		Error
	};

	/**
	 * Progress function signature.
	 */
	typedef std::function<void(std::string, ProgressType)> ProgressFunction;

	/**
	 * Set the progress function for the writer.
	 *
	 * \param[in] new_func The new function to use.
	 */
	void SetProgressFunction(ProgressFunction new_func);

	/**
	 * Write the provided SHPSCore image to a PNG file.
	 *
	 * Returns false on failure (progress function will output a possible reason),
	 * or true on successful image write..
	 *
	 * \param[in] image Image to write.
	 * \param[in] input_path The input path. Must contain a filename.
	 * \param[in] output_path The output path.
	 */
	bool WriteImage(core::ShpsImage& image, const std::filesystem::path& input_path, const std::filesystem::path& output_path);
	
}
}