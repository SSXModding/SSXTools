#pragma once

#include <iostream>
#include <vector>
#include <exception>
#include <fstream>
#include "ShpsHeader.h"

// Extracts texture from STL stream.
class TextureExtractor {
	std::istream& m_stream;
	ShpsHeader m_header;
	std::vector<char> m_textureData;
public:
	TextureExtractor(std::istream& stream) : m_stream(stream) {}

	void Extract();
};