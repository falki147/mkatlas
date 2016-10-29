// Disable warnings for fopen
#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include "Image.hpp"

#include <png.h>
#include <memory>
#include <functional>
#include <cassert>

#include "Utils.hpp"

// Stores the message from callback function
thread_local static const char* message = nullptr;

Image Image::load(const std::string& file) {
	auto f = finalize(fopen(file.c_str(), "rb"), fclose);

	if (!f)
		throw std::runtime_error(combine("failed to open file (\"", file, "\")"));

	png_byte sig[8];
		
	if (!fread(sig, 8, 1, f.get()) || !png_check_sig(sig, 8))
		throw std::runtime_error(combine("invalid png file (\"", file, "\")"));

	message = nullptr;

	auto png = finalize(png_create_read_struct(
		PNG_LIBPNG_VER_STRING, nullptr,
		[](auto png, auto msg) { message = msg; longjmp(png_jmpbuf(png), 1); },
		nullptr
	), [](auto png) { png_destroy_read_struct(&png, nullptr, nullptr); });

	if (!png)
		throw std::bad_alloc();

	if (setjmp(png_jmpbuf(png.get()))) {
		if (!message)
			throw std::runtime_error(combine("libpng failed (\"", file, "\")"));

		throw std::runtime_error(combine("loading png file failed (\"", file, "\"): ", message));
	}

	auto info = finalize(png_create_info_struct(png.get()), [](auto info) { png_destroy_read_struct(nullptr, &info, nullptr); });

	if (!info)
		throw std::bad_alloc();

	png_init_io(png.get(), f.get());
	png_set_sig_bytes(png.get(), 8);

	png_read_info(png.get(), info.get());

	Image img(png_get_image_width(png.get(), info.get()), png_get_image_height(png.get(), info.get()));

	auto type = png_get_color_type(png.get(), info.get());
	auto depth = png_get_bit_depth(png.get(), info.get());

	if (depth == 16)
		png_set_strip_16(png.get());

	if (type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png.get());

	if (type == PNG_COLOR_TYPE_GRAY && depth < 8)
		png_set_expand_gray_1_2_4_to_8(png.get());

	if (png_get_valid(png.get(), info.get(), PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png.get());

	if (type == PNG_COLOR_TYPE_RGB ||
		type == PNG_COLOR_TYPE_GRAY ||
		type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png.get(), 0xFF, PNG_FILLER_AFTER);

	if (type == PNG_COLOR_TYPE_GRAY ||
		type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png.get());

	png_read_update_info(png.get(), info.get());

	auto rowBytes = png_get_rowbytes(png.get(), info.get());
	img.m_data.resize(rowBytes * img.m_height);

	std::unique_ptr<png_bytep[]> rowPointers(new png_bytep[img.m_height]);

	for (unsigned int i = 0; i < img.m_height; i++)
		rowPointers[i] = (png_bytep) img.data() + i * rowBytes;

	png_read_image(png.get(), rowPointers.get());

	return img;
}

void Image::save(const std::string& file) const {
	auto f = finalize(fopen(file.c_str(), "wb"), fclose);

	if (!f)
		throw std::runtime_error(combine("failed to open file (\"", file, "\")"));

	message = nullptr;
	
	auto png = finalize(png_create_write_struct(
		PNG_LIBPNG_VER_STRING, nullptr,
		[](auto png, auto msg) { message = msg; longjmp(png_jmpbuf(png), 1); },
		nullptr
	), [](auto png) { png_destroy_write_struct(&png, nullptr); });

	if (!png)
		throw std::bad_alloc();

	if (setjmp(png_jmpbuf(png.get()))) {
		if (!message)
			throw std::runtime_error(combine("libpng failed (\"", file, "\")"));

		throw std::runtime_error(combine("saving png file failed (\"", file, "\"): ", message));
	}

	auto info = finalize(png_create_info_struct(png.get()), [](auto info) { png_destroy_write_struct(nullptr, &info); });

	if (!info)
		throw std::bad_alloc();

	png_set_IHDR(
		png.get(), info.get(), m_width, m_height, 8,
		PNG_COLOR_TYPE_RGBA,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);

	auto rowBytes = 4 * m_width;

	std::unique_ptr<png_bytep[]> rowPointers(new png_bytep[m_height]);

	for (unsigned int i = 0; i < m_height; i++)
		rowPointers[i] = (png_bytep) m_data.data() + i * rowBytes;

	png_init_io(png.get(), f.get());
	png_set_rows(png.get(), info.get(), rowPointers.get());
	png_write_png(png.get(), info.get(), PNG_TRANSFORM_IDENTITY, nullptr);
}

Rectangle Image::getBounds() const {
	unsigned int x1 = 0, y1 = 0, x2 = m_width - 1, y2 = m_height - 1;

	// Search for upper border
	for (; y1 < m_height; ++y1) {
		unsigned int i;

		for (i = 0; i < m_width; ++i)
			if (at(i, y1) & 0xFF000000)
				break;

		if (i < m_width)
			break;
	}

	// If y1 is greater equal height the image is empty
	if (y1 >= m_height)
		return { 0, 0, 0, 0 };

	// Search for lower border
	for (; y2 > y1; --y2) {
		unsigned int i;

		for (i = 0; i < m_width; ++i)
			if (at(i, y2) & 0xFF000000)
				break;

		if (i < m_width)
			break;
	}

	// Search for left border
	for (; x1 < m_width; ++x1) {
		unsigned int i;

		for (i = y1; i <= y2; ++i)
			if (at(x1, i) & 0xFF000000)
				break;

		if (i <= y2)
			break;
	}

	// Search for lower border
	for (; x2 > x1; --x2) {
		unsigned int i;

		for (i = y1; i <= y2; ++i)
			if (at(x2, i) & 0xFF000000)
				break;

		if (i <= y2)
			break;
	}

	return { x1, y1, x2 - x1 + 1, y2 - y1 + 1 };
}

void Image::copy(const Image& img, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int dx, unsigned int dy) {
	assert(x + w <= img.width());
	assert(y + h <= img.height());
	assert(dx + w <= width());
	assert(dy + h <= height());

	auto pointerSrc = &img.at(x, y);
	auto strideSrc = img.width() - w;

	auto pointerDst = &at(dx, dy);
	auto strideDst = width() - w;

	for (unsigned int j = 0; j < h; ++j) {
		for (unsigned int i = 0; i < w; ++i)
			*pointerDst++ = *pointerSrc++;

		pointerSrc += strideSrc;
		pointerDst += strideDst;
	}
}

void Image::copyFlipped(const Image& img, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int dx, unsigned int dy) {
	assert(x + w <= img.width());
	assert(y + h <= img.height());

	// h and w swapped because the image gets flipped
	assert(dx + h <= width());
	assert(dy + w <= height());

	auto pointerSrc = &img.atFlipped(x, y);
	auto strideSrc = h * img.width() + 1;
	auto innerStrideSrc = img.width();

	auto pointerDst = &at(dx, dy);
	auto strideDst = width() - h;

	for (unsigned int j = 0; j < w; ++j) {
		for (unsigned int i = 0; i < h; ++i) {
			*pointerDst++ = *pointerSrc;
			pointerSrc += innerStrideSrc;
		}

		pointerSrc -= strideSrc;
		pointerDst += strideDst;
	}
}

void Image::fill(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int color) {
	assert(x + w <= width());
	assert(y + h <= height());

	auto pointerDst = &at(x, y);
	auto strideDst = width() - w;

	for (unsigned int j = 0; j < h; ++j) {
		for (unsigned int i = 0; i < w; ++i)
			*pointerDst++ = color;
		
		pointerDst += strideDst;
	}
}

void Image::copyLineHor(const Image& img, unsigned int x, unsigned int y, unsigned int length, unsigned int dx, unsigned int dy) {
	assert(x + length <= img.width());
	assert(y < img.height());
	assert(dx + length <= width());
	assert(dy < height());

	auto pointerSrc = &img.at(x, y);
	auto pointerDst = &at(dx, dy);

	for (unsigned int i = 0; i < length; ++i)
		*pointerDst++ = *pointerSrc++;
}

void Image::copyLineVert(const Image& img, unsigned int x, unsigned int y, unsigned int length, unsigned int dx, unsigned int dy) {
	assert(x < img.width());
	assert(y + length <= img.height());
	assert(dx < width());
	assert(dy + length <= height());

	auto pointerSrc = &img.at(x, y);
	auto strideSrc = img.width();

	auto pointerDst = &at(dx, dy);
	auto strideDst = width();

	for (unsigned int i = 0; i < length; ++i) {
		*pointerDst = *pointerSrc;

		pointerSrc += strideSrc;
		pointerDst += strideDst;
	}
}

void Image::copyLineHorFlipped(const Image& img, unsigned int x, unsigned int y, unsigned int length, unsigned int dx, unsigned int dy) {
	assert(x + length <= img.height());
	assert(y < img.width());
	assert(dx + length <= width());
	assert(dy < height());

	auto pointerSrc = &img.atFlipped(x, y);
	auto strideSrc = img.width();

	auto pointerDst = &at(dx, dy);

	for (unsigned int i = 0; i < length; ++i) {
		*pointerDst++ = *pointerSrc;
		pointerSrc += strideSrc;
	}
}

void Image::copyLineVertFlipped(const Image& img, unsigned int x, unsigned int y, unsigned int length, unsigned int dx, unsigned int dy) {
	assert(x < img.height());
	assert(y + length <= img.width());
	assert(dx < width());
	assert(dy + length <= height());

	auto pointerSrc = &img.atFlipped(x, y);

	auto pointerDst = &at(dx, dy);
	auto strideDst = width();

	for (unsigned int i = 0; i < length; ++i) {
		*pointerDst = *pointerSrc--;
		pointerDst += strideDst;
	}
}
