#include "Utils.hpp"

unsigned int makeRGBFromString(const std::string& color, unsigned char alpha) {
	auto num = std::stoul(color, nullptr, 16);
	return makeRBGA((unsigned char) (num >> 16), (unsigned char) (num >> 8), (unsigned char) num, alpha);
}

void writeJSONString(std::ostream& stream, const std::string& value) {
	const char* table[] = {
		"\\u0000", "\\u0001", "\\u0002", "\\u0003", "\\u0004", "\\u0005", "\\u0006", "\\u0007",
		"\\b",     "\\t",     "\\n",     "\\u0011", "\\f",     "\\r",     "\\u0014", "\\u0015",
		"\\u0016", "\\u0017", "\\u0018", "\\u0019", "\\u0020", "\\u0021", "\\u0022", "\\u0023",
		"\\u0024", "\\u0025", "\\u0026", "\\u0027", "\\u0028", "\\u0029", "\\u0030", "\\u0031",
	};

	stream << '"';

	auto last = value.begin();

	for (auto it = value.begin(); it != value.end(); ++it) {
		if (*it == '"') {
			stream.write(&*last, it - last);
			stream << "\\\"";
			last = std::next(it);
		}
		else if (*it == '\\') {
			stream.write(&*last, it - last);
			stream << "\\\\";
			last = std::next(it);
		}
		else if (*it < 0x20) {
			stream.write(&*last, it - last);
			stream << table[*it];
			last = std::next(it);
		}
	}

	stream << '"';
}
