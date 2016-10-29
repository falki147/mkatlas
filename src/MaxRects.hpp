#pragma once

#include <vector>
#include <list>

struct RectData {
	unsigned int m_x, m_y, m_w, m_h;
	bool m_flipped;
	unsigned int m_bin;
};

class MaxRects {
public:
	struct Configuration {
		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_maxBins;
		bool m_canFlip;
	};

	MaxRects(const Configuration& config);
	~MaxRects();
		
	inline void configure(const Configuration& config) {
		m_config = config;
		clear();
	}

	inline const Configuration& getConfiguration() const {
		return m_config;
	}

	inline unsigned int getNumBins() const {
		return m_bins.size();
	}

	void add(RectData* data, unsigned int w, unsigned int h);
	void clear();
	bool pack();

private:
	struct Rect {
		unsigned int m_x, m_y, m_w, m_h;
	};

	struct Bin {
		bool m_full;
		std::list<Rect> m_freeRects;
	};

	typedef std::vector<Bin>::iterator BinIt;
	typedef std::list<RectData*>::iterator RectDataIt;
	typedef std::list<Rect>::iterator RectIt;

	static inline unsigned long long combine(unsigned int x, unsigned int y) {
		return (long long) x << 32 | y;
	}

	bool findBest(BinIt& outBin, RectIt& outFreeRect, RectDataIt& outRect, bool& flip);
	unsigned long long getScore(const Rect& dest, unsigned int w, unsigned int h) const;
		
	std::list<RectData*> m_rectangles;
	std::vector<Bin> m_bins;

	Configuration m_config;
};
