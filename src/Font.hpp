#pragma once

#include <string>
#include <vector>
#include <ft2build.h>
#include <freetype/freetype.h>

#include "Image.hpp"
#include "Range.hpp"

struct Glyph {
	unsigned int m_ind;
	int transX;
	int transY;
	float advX;
	float advY;
	Image m_img;
};

struct Font {
	std::vector<Glyph> m_glyphs;
};

class FreeType {
public:
	FreeType();
	~FreeType();

	Font load(const std::string& file, unsigned int size, unsigned int color, const std::vector<Range>& ranges);

private:
	FT_Library m_lib = nullptr;
};
