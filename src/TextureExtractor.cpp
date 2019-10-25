#include <cstring>
#include <sstream>
#include "UtilStuff.h"
#include "TextureExtractor.h"

bool CheckValidHeader(ShpsHeader& header) {
	// tools already exist for .FSH
	if (SizedCmp(header.Magic, "SHPS")) { 
		switch (header.FileVersion) {
			default:
				return false;
			break;

			case ShpsVersion::Version1:
			break;
		}

		if (SizedCmp(header.FileMagic, "GIMX")) {
			if (SizedCmpOff(header.GroupName, "Buy", 1)) {
				if (SizedCmp(header.ErtsMagic, "ERTS")) {
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
	std::cout << "File size: " << m_header.FileLength << " bytes" << '\n';
	std::cout << "Texture Format Version: " << m_header.FileVersion << '\n';
	
	std::cout << "File code: "; 
	PrintFixedArray(m_header.FileName, sizeof(m_header.FileName), std::cout); 
	std::cout << '\n';

	std::cout << "Image size: " << m_header.ImageWidth << 'x' << m_header.ImageHeight << '\n';
	std::cout << "Getting texture data\n";

	if (!m_stream.eof()) {
		std::stringstream stream;
		stream << m_stream.rdbuf();
		std::string content = stream.str();
		m_textureData = std::vector<char>(content.begin(), content.end());
		stream.clear();
	}

	std::cout << "Texture data output:\n";
	std::cout << m_textureData.data();
	std::cout << '\n';
}