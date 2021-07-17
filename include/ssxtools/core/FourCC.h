//
// Created by lily on 7/14/21.
//

#ifndef SSXTOOLS_FOURCC_H
#define SSXTOOLS_FOURCC_H

#include <cstdint>

namespace ssxtools::core {

	/**
	 * Make a FourCC value at compile time, with a given input string.
	 */
	constexpr std::uint32_t FourCCValue(const char fourCc[5]) {
		return (fourCc[0] << 24) | (fourCc[1] << 16) | (fourCc[2] << 8) | fourCc[3];
	}

}

#endif //SSXTOOLS_FOURCC_H
