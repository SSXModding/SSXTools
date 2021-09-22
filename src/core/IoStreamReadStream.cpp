#include <ssxtools/core/IoStreamReadStream.h>

namespace ssxtools::core {

	bool IoStreamReadStream::Seek(StreamOff pos, StreamSeekDirection dir) {
		if(!stream)
			return false;

		switch(dir) {
			case StreamSeekDirection::Begin:
				stream.seekg(pos, std::istream::beg);
				break;
			case StreamSeekDirection::Current:
				stream.seekg(pos, std::istream::cur);
				break;
			case StreamSeekDirection::End:
				stream.seekg(pos, std::istream::end);
				break;
		}

		return true;
	}

	StreamOff IoStreamReadStream::Tell() const {
		if(!stream)
			return -1;

		return stream.tellg();
	}

	bool IoStreamReadStream::Byte(std::uint8_t& b) {
		if(!stream.get(reinterpret_cast<char&>(b)))
			return false;
		return true;
	}


	bool IoStreamReadStream::String(std::string& string) {
		if(!stream)
			return false;

		// This isn't terribly performant, but there really isn't much that needs
		// zero terminated string reading, so it's fine.
		char c;
		while(true) {
			if(!stream.get(c))
				return false;

			if(c == '\0')
				break;

			// while += c does the same thing as what I'm doing here (unlike the travesty that is the CLR's System.String type in this regard),
			// this is slightly more direct.
			string.push_back(c);
		}
		return true;
	}

	/**
	 * Get the wrapped stream.
	 */
	std::istream& IoStreamReadStream::GetStream() const {
		return stream;
	}

	// This pre-instantiates all of the stream methods, so they're free to use for anything.
	// Anything not used will get linked out when built as Release, anyways.
#define TYPE(FuncName, T) \
	template bool IoStreamReadStream::FuncName<std::endian::little>(T&); \
	template bool IoStreamReadStream::FuncName<std::endian::big>(T&);
#include <ssxtools/core/StreamTypeListing.inl>
#undef TYPE

}
