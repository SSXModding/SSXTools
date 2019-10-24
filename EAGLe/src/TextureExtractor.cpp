#include <cstring>
#include "utility.h"
#include "TextureExtractor.h"

bool CheckValidHeader(ShpsHeader& header) {
	// tools already exist for .FSH
	if (!strncmp(header.Magic, "SHPS", 3)) { 

		switch (header.FileVersion) {
			default:
				return false;
			break;

			case ShpsVersion::Version1:
			break;
		}

		if (!strncmp(header.FileMagic, "GIMX", 4)) {
			if (!strncmp(header.GroupName, "Buy", 3)) {
				if (!strncmp(header.ErtsMagic, "ERTS", 4)) {
					return true;
				} else {
					return false;
				}
			} else {
				return false;
			}
		} else { 
			return false; 
		}
	} else {
		return false;
	}
}

void TextureExtractor::Extract() {
	ReadStruct<ShpsHeader>(m_stream, m_header);
	
	if (!CheckValidHeader(m_header))
		throw std::invalid_argument("EAGL SSH header invalid");

	std::cout << "Texture Info:" << '\n';
	
	std::cout << "File size: " << m_header.FileLength << "bytes" << '\n';
	std::cout << "Texture Format Version: " << m_header.FileVersion << '\n';
	
	std::cout << "File code: "; 
	PrintFixedArray(m_header.FileName, sizeof(m_header.FileName), std::cout); 
	std::cout << '\n';

	std::cout << "Image size: " << m_header.ImageWidth << 'x' << m_header.ImageHeight << '\n';
	
	std::cout << "Getting texture data\n";

	m_textureData.resize(m_header.FileLength - sizeof(ShpsHeader));

	char byte;
	while (!m_stream.eof()) {
		m_stream.read(&byte, sizeof(char));
		m_textureData.push_back(byte);
	}


	std::cout << "Texture data output:\n";
	for (char& c : m_textureData) {
		std::cout << c;
	}
	std::cout << '\n';
}