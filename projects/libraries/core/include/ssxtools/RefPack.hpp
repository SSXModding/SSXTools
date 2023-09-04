#pragma once
#include <optional>
#include <ssxtools/Types.hpp>
#include <vector>

namespace ssxtools::core {

	std::optional<std::vector<u8>> RefPackDecompress(const std::vector<u8>& data);

}
