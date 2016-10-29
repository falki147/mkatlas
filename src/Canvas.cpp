#include "Canvas.hpp"

#include <algorithm>

Canvas::Canvas(unsigned int width, unsigned int height): m_img(width, height) { }

void Canvas::draw(const Image& img, unsigned int x, unsigned int y, bool flip, unsigned int expand) {
	auto width = img.width(), height = img.height();

	if (width == 0 || height == 0)
		return;

	if (expand > 0) {
		auto padding = expand >> 1;
		auto otherPadding = expand - padding;

		if (flip) {
			m_img.copyFlipped(img, 0, 0, width, height, x + padding, y + padding);

			for (unsigned int i = 0; i < padding; ++i) {
				m_img.copyLineHorFlipped(img, 0, 0, height, x + padding, y + i);
				m_img.copyLineVertFlipped(img, 0, 0, width, x + i, y + padding);
			}

			for (unsigned int i = 0; i < otherPadding; ++i) {
				m_img.copyLineHorFlipped(img, 0, width - 1, height, x + padding, y + padding + width + i);
				m_img.copyLineVertFlipped(img, height - 1, 0, width, x + padding + height + i, y + padding);
			}

			m_img.fill(x, y, padding, padding, img.atFlipped(0, 0));
			m_img.fill(x + padding + height, y, otherPadding, padding, img.atFlipped(height - 1, 0));
			m_img.fill(x + padding + height, y + padding + width, otherPadding, otherPadding, img.atFlipped(height - 1, width - 1));
			m_img.fill(x, y + padding + width, padding, otherPadding, img.atFlipped(0, width - 1));
		}
		else {
			m_img.copy(img, 0, 0, width, height, x + padding, y + padding);

			for (unsigned int i = 0; i < padding; ++i) {
				m_img.copyLineHor(img, 0, 0, width, x + padding, y + i);
				m_img.copyLineVert(img, 0, 0, height, x + i, y + padding);
			}

			for (unsigned int i = 0; i < otherPadding; ++i) {
				m_img.copyLineHor(img, 0, height - 1, width, x + padding, y + padding + height + i);
				m_img.copyLineVert(img, width - 1, 0, height, x + padding + width + i, y + padding);
			}

			m_img.fill(x, y, padding, padding, img.at(0, 0));
			m_img.fill(x + padding + width, y, otherPadding, padding, img.at(width - 1, 0));
			m_img.fill(x + padding + width, y + padding + height, otherPadding, otherPadding, img.at(width - 1, height - 1));
			m_img.fill(x, y + padding + height, padding, otherPadding, img.at(0, height - 1));
		}
	}
	else if (flip)
		m_img.copyFlipped(img, 0, 0, width, height, x, y);
	else
		m_img.copy(img, 0, 0, width, height, x, y);
}

void Canvas::drawRect(const Image& img, const Rectangle& rect, unsigned int x, unsigned int y, bool flip, unsigned int expand) {
	auto width = rect.m_w, height = rect.m_h;

	if (width == 0 || height == 0)
		return;

	if (expand > 0) {
		auto padding = expand >> 1;
		auto otherPadding = expand - padding;

		if (flip) {
			m_img.copyFlipped(img, rect.m_x, rect.m_y, width, height, x + padding, y + padding);

			for (unsigned int i = 0; i < padding; ++i) {
				m_img.copyLineHorFlipped(img, rect.m_x, rect.m_y, height, x + padding, y + i);
				m_img.copyLineVertFlipped(img, rect.m_x, rect.m_y, width, x + i, y + padding);
			}

			for (unsigned int i = 0; i < otherPadding; ++i) {
				m_img.copyLineHorFlipped(img, rect.m_x, rect.m_y + width - 1, height, x + padding, y + padding + width + i);
				m_img.copyLineVertFlipped(img, rect.m_x + height - 1, rect.m_y, width, x + padding + height + i, y + padding);
			}

			m_img.fill(x, y, padding, padding, img.atFlipped(rect.m_x, rect.m_y));
			m_img.fill(x + padding + height, y, otherPadding, padding, img.atFlipped(rect.m_x + height - 1, rect.m_y));
			m_img.fill(x + padding + height, y + padding + width, otherPadding, otherPadding, img.atFlipped(rect.m_x + height - 1, rect.m_y + width - 1));
			m_img.fill(x, y + padding + width, padding, otherPadding, img.atFlipped(rect.m_x, rect.m_y + width - 1));
		}
		else {
			m_img.copy(img, rect.m_x, rect.m_y, width, height, x + padding, y + padding);

			for (unsigned int i = 0; i < padding; ++i) {
				m_img.copyLineHor(img, rect.m_x, rect.m_y, width, x + padding, y + i);
				m_img.copyLineVert(img, rect.m_x, rect.m_y, height, x + i, y + padding);
			}

			for (unsigned int i = 0; i < otherPadding; ++i) {
				m_img.copyLineHor(img, rect.m_x, rect.m_y + height - 1, width, x + padding, y + padding + height + i);
				m_img.copyLineVert(img, rect.m_x + width - 1, rect.m_y, height, x + padding + width + i, y + padding);
			}

			m_img.fill(x, y, padding, padding, img.at(rect.m_x, rect.m_y));
			m_img.fill(x + padding + width, y, otherPadding, padding, img.at(rect.m_x + width - 1, rect.m_y));
			m_img.fill(x + padding + width, y + padding + height, otherPadding, otherPadding, img.at(rect.m_x + width - 1, rect.m_y + height - 1));
			m_img.fill(x, y + padding + height, padding, otherPadding, img.at(rect.m_x, rect.m_y + height - 1));
		}
	}
	else if (flip)
		m_img.copyFlipped(img, rect.m_x, rect.m_y, width, height, x, y);
	else
		m_img.copy(img, rect.m_x, rect.m_y, width, height, x, y);
}
