#pragma once

#include <iostream>
#include <exception>
#include <fstream>
#include "eagl_ps2.h"

// Extracts texture from STL stream.
class TextureExtractor {
	std::istream& m_stream;
	ShpsHeader m_header;
public:
	TextureExtractor(std::istream& stream) : m_stream(stream) {}

	void Extract();
};