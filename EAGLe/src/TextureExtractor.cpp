#include <cstring>
#include "utility.h"
#include "TextureExtractor.h"

// Test for a valid SHPS/SSH header
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
		throw std::exception("SSH header invalid");

	std::cout << "EAGL Texture Info:\n";
	std::cout << "File Size: " << m_header.FileLength << '\n';
	std::cout << "Texture Format Version: " << m_header.FileVersion << '\n';
	std::cout << "Filecode: "; PrintFixedArray(m_header.FileName, sizeof(m_header.FileName), std::cout); std::cout << '\n';

	std::cout << "Image Size: " << m_header.ImageWidth << 'x' << m_header.ImageHeight << '\n';
}