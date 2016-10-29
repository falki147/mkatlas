#include "DistantField.hpp"

#include <algorithm>

#include "Utils.hpp"

static bool isInside(unsigned int color) {
	return (color & 0x80000000) != 0;
}

static inline int clamp(int val, int min, int max) {
	return std::max(std::min(val, max), min);
}

static inline void distMin(int& minDist, int dx, int dy) {
	const auto dist = dx * dx + dy * dy;

	if (dist < minDist)
		minDist = dist;
}

static float findSignedDistance(const std::vector<bool>& map, int x, int y, int w, int h, float spread) {
	auto ins = (x < 0 || x >= w || y < 0 || y >= h) ? false : map[y * w + x];

	auto delta = (int) std::ceil(spread);

	auto startX = clamp(x - delta, 0, w - 1);
	auto endX   = clamp(x + delta, 0, w - 1);
	auto startY = clamp(y - delta, 0, h - 1);
	auto endY   = clamp(y + delta, 0, h - 1);

	auto minDist = delta * delta;

	if (ins) {
		distMin(minDist, x + 1, 0);
		distMin(minDist, w - x, 0);
		distMin(minDist, y + 1, 0);
		distMin(minDist, h - y, 0);
	}

	for (auto j = startY; j <= endY; ++j)
		for (auto i = startX; i <= endX; ++i)
			if (ins != map[j * w + i])
				distMin(minDist, x - i, y - j);

	auto dist = std::min(std::sqrtf((float) minDist), spread) / spread;
	return ins ? dist : -dist;
}

static unsigned int distanceToRGB(float dist) {
	auto v = std::max(std::min(0.5f + 0.5f * dist, 1.0f), 0.0f);
	auto vc = (unsigned char) (v * 0xFF);
	return makeRBGA(vc, vc, vc, 0xFF);
}

Image distantFieldFromImage(const Image& src, unsigned int spread, unsigned int downscale) {
	if (src.empty())
		return src;

	auto padding = spread / downscale;

	Image img(src.width() / downscale + 2 * padding, src.height() / downscale + 2 * padding);

	std::vector<bool> map;
	map.reserve(src.width() * src.height());

	for (unsigned int i = 0; i < src.width() * src.height(); ++i)
		map.push_back(isInside(src.data()[i]));

	for (int j = 0; j < (int) img.height(); ++j) {
		for (int i = 0; i < (int) img.width(); ++i) {
			auto x = ((i - padding) * downscale) + (downscale / 2);
			auto y = ((j - padding) * downscale) + (downscale / 2);
			
			img.at(i, j) = distanceToRGB(
				findSignedDistance(map, x, y, src.width(), src.height(), (float) spread)
			);
		}
	}

	return img;
}
