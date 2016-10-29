#include "Utils.hpp"

unsigned int makeRGBFromString(const std::string& color, unsigned char alpha) {
	auto num = std::stoul(color, nullptr, 16);
	return makeRBGA((unsigned char) (num >> 16), (unsigned char) (num >> 8), (unsigned char) num, alpha);
}
