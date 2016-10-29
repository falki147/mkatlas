#include "MaxRects.hpp"

#include <algorithm>
#include <stdexcept>

static inline bool fits(unsigned int w, unsigned int h, unsigned int targetW, unsigned int targetH, bool canFlip) {
	return canFlip ? ((w <= targetW && h <= targetH) || (h <= targetW && w <= targetH)) : (w <= targetW && h <= targetH);
}

MaxRects::MaxRects(const Configuration& config): m_config(config) { }

MaxRects::~MaxRects() {
	clear();
}

void MaxRects::add(RectData* data, unsigned int w, unsigned int h) {
	if (!fits(w, h, m_config.m_width, m_config.m_height, m_config.m_canFlip))
		throw std::runtime_error("rectangle doesn't fit");

	*data = { 0, 0, w, h, false, 0 };

	if (data->m_w != 0 && data->m_h != 0)
		m_rectangles.push_back(data);
}

void MaxRects::clear() {
	m_rectangles.clear();
	m_bins.clear();
}

bool MaxRects::findBest(BinIt& outBin, RectIt& outFreeRect, RectDataIt& outRect, bool& flip) {
	auto bestScore = std::numeric_limits<unsigned long long>::max();

	for (auto binIt = m_bins.begin(); binIt != m_bins.end(); ++binIt) {
		auto& bin = *binIt;

		if (bin.m_full)
			continue;

		for (auto freeRect = bin.m_freeRects.begin(); freeRect != bin.m_freeRects.end(); ++freeRect) {
			for (auto rectIt = m_rectangles.begin(); rectIt != m_rectangles.end(); ++rectIt) {
				auto& rect = *rectIt;

				if (rect->m_w <= freeRect->m_w && rect->m_h <= freeRect->m_h) {
					auto score = getScore(*freeRect, rect->m_w, rect->m_h);

					if (score < bestScore) {
						outBin      = binIt;
						outFreeRect = freeRect;
						outRect     = rectIt;
						flip        = false;

						bestScore = score;
					}
				}
				
				if (m_config.m_canFlip && rect->m_h <= freeRect->m_w && rect->m_w <= freeRect->m_h) {
					auto score = getScore(*freeRect, rect->m_h, rect->m_w);

					if (score < bestScore) {
						outBin      = binIt;
						outFreeRect = freeRect;
						outRect     = rectIt;
						flip        = true;

						bestScore = score;
					}
				}
			}
		}
	}

	return bestScore != std::numeric_limits<unsigned long long>::max();
}

unsigned long long MaxRects::getScore(const Rect& dest, unsigned int w, unsigned int h) const {
	return combine(dest.m_w * dest.m_h - w * h, std::min(dest.m_w - w, dest.m_h - h));
}

bool MaxRects::pack() {
	m_bins.clear();

	m_bins.push_back({
		false, std::list<Rect>(1, { 0, 0, m_config.m_width, m_config.m_height })
	});

	while (!m_rectangles.empty()) {
		BinIt bin;
		RectIt freeRect;
		RectDataIt rect;
		bool flip;

		// If it couldn't find a free spot, add bin
		if (!findBest(bin, freeRect, rect, flip)) {
			// Can't add a new bin
			if (m_config.m_maxBins > 0 && m_bins.size() >= (unsigned int) m_config.m_maxBins) {
				for (auto& rect : m_rectangles)
					rect->m_bin = std::numeric_limits<unsigned int>::max();

				return false;
			}

			for (auto& bin: m_bins)
				bin.m_full = true;

			m_bins.push_back({
				false, std::list<Rect>(1, { 0, 0, m_config.m_width, m_config.m_height })
			});
			continue;
		}

		// Update rect data
		auto& rectRef = **rect;

		rectRef.m_x = freeRect->m_x;
		rectRef.m_y = freeRect->m_y;
		rectRef.m_flipped = flip;
		rectRef.m_bin = std::distance(m_bins.begin(), bin);

		// Split intersecting rectangles
		std::list<Rect> newRects;

		const auto width  = flip ? rectRef.m_h : rectRef.m_w;
		const auto height = flip ? rectRef.m_w : rectRef.m_h;

		bin->m_freeRects.remove_if([&newRects, &rectRef, &width, &height](Rect& freeRect) {
			if (rectRef.m_x >= freeRect.m_x + freeRect.m_w || rectRef.m_x + width <= freeRect.m_x ||
				rectRef.m_y >= freeRect.m_y + freeRect.m_h || rectRef.m_y + height <= freeRect.m_y)
				return false;

			if (rectRef.m_x < freeRect.m_x + freeRect.m_w && rectRef.m_x + width > freeRect.m_x) {
				if (rectRef.m_y > freeRect.m_y && rectRef.m_y < freeRect.m_y + freeRect.m_h)
					newRects.push_back({ freeRect.m_x, freeRect.m_y, freeRect.m_w, rectRef.m_y - freeRect.m_y });

				if (rectRef.m_y + height < freeRect.m_y + freeRect.m_h)
					newRects.push_back({ freeRect.m_x, rectRef.m_y + height, freeRect.m_w, freeRect.m_y + freeRect.m_h - (rectRef.m_y + height) });
			}

			if (rectRef.m_y < freeRect.m_y + freeRect.m_h && rectRef.m_y + height > freeRect.m_y) {
				if (rectRef.m_x > freeRect.m_x && rectRef.m_x < freeRect.m_x + freeRect.m_w)
					newRects.push_back({ freeRect.m_x, freeRect.m_y, rectRef.m_x - freeRect.m_x, freeRect.m_h });

				if (rectRef.m_x + width < freeRect.m_x + freeRect.m_w)
					newRects.push_back({ rectRef.m_x + width, freeRect.m_y, freeRect.m_x + freeRect.m_w - (rectRef.m_x + width), freeRect.m_h });
			}

			return true;
		});

		newRects.remove_if([&bin](Rect& rect) {
			for (auto& other : bin->m_freeRects)
				if (rect.m_x >= other.m_x && rect.m_y >= other.m_y &&
					rect.m_x + rect.m_w <= other.m_x + other.m_w &&
					rect.m_y + rect.m_h <= other.m_y + other.m_h)
					return true;

			return false;
		});

		bin->m_freeRects.remove_if([&newRects](Rect& rect) {
			for (auto& other : newRects)
				if (rect.m_x >= other.m_x && rect.m_y >= other.m_y &&
					rect.m_x + rect.m_w <= other.m_x + other.m_w &&
					rect.m_y + rect.m_h <= other.m_y + other.m_h)
					return true;

			return false;
		});

		bin->m_freeRects.splice(bin->m_freeRects.end(), newRects);
		
		// Remove rect
		m_rectangles.erase(rect);
	}

	return true;
}
