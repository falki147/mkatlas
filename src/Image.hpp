#pragma once

#include "Rectangle.hpp"

#include <string>
#include <vector>

class Image {
public:
	Image(unsigned int width, unsigned int height): m_width(width), m_height(height), m_data(width * height) { }

	static Image load(const std::string& file);
	void save(const std::string& file) const;

	inline unsigned int width() const {
		return m_width;
	}

	inline unsigned int height() const {
		return m_height;
	}

	inline bool empty() const {
		return m_data.empty();
	}

	inline unsigned int* data() {
		return m_data.data();
	}

	inline const unsigned int* data() const {
		return m_data.data();
	}

	inline unsigned int& at(unsigned int x, unsigned int y) {
		return m_data[y * m_width + x];
	}

	inline const unsigned int& at(unsigned int x, unsigned int y) const {
		return m_data[y * m_width + x];
	}

	inline unsigned int& atFlipped(unsigned int x, unsigned int y) {
		return at(m_width - y - 1, x);
	}

	inline const unsigned int& atFlipped(unsigned int x, unsigned int y) const {
		return at(m_width - y - 1, x);
	}

	Rectangle getBounds() const;

	void copy(const Image& img, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int dx, unsigned int dy);
	void copyFlipped(const Image& img, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int dx, unsigned int dy);
	void fill(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int color);
	void copyLineHor(const Image& img, unsigned int x, unsigned int y, unsigned int length, unsigned int dx, unsigned int dy);
	void copyLineVert(const Image& img, unsigned int x, unsigned int y, unsigned int length, unsigned int dx, unsigned int dy);
	void copyLineHorFlipped(const Image& img, unsigned int x, unsigned int y, unsigned int length, unsigned int dx, unsigned int dy);
	void copyLineVertFlipped(const Image& img, unsigned int x, unsigned int y, unsigned int length, unsigned int dx, unsigned int dy);

private:
	unsigned int m_width;
	unsigned int m_height;
	std::vector<unsigned int> m_data;
};
