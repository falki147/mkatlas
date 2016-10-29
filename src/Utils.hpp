#pragma once

#include <string>
#include <memory>

unsigned int constexpr makeRBGA(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	return r | (g << 8) | (b << 16) | (a << 24);
}

template<typename... Args> inline std::string combine(const std::string& str) {
	return str;
}

template<typename... Args> inline std::string combine(const std::string& str, Args... args) {
	return str + combine(args...);
}

template<typename T, typename Pred> auto finalize(T* value, Pred pred) {
	return std::unique_ptr<T, Pred>(value, pred);
}

unsigned int makeRGBFromString(const std::string& color, unsigned char alpha = 0xFF);
