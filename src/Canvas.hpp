#pragma once

#include "Image.hpp"

class Canvas {
public:
	Canvas(unsigned int width, unsigned int height);

	void draw(const Image& img, unsigned int x, unsigned int y, bool flip, unsigned int expand);
	void drawRect(const Image& img, const Rectangle& rect, unsigned int x, unsigned int y, bool flip, unsigned int expand);

	inline const Image& getImage() const {
		return m_img;
	}

private:
	Image m_img;
};
