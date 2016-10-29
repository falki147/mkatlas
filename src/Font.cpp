#ifndef DISABLE_FREETYPE
#include "Font.hpp"

#include "Utils.hpp"

static Image imageFromSlot(const FT_GlyphSlot slot, unsigned int color) {
	if (slot->bitmap.width != 0 && slot->bitmap.rows != 0) {
		Image img(slot->bitmap.width, slot->bitmap.rows);

		for (unsigned int j = 0; j < img.width() * img.height(); ++j)
			img.data()[j] = slot->bitmap.buffer[j] << 24 | color;

		return img;
	}

	return Image(0, 0);
}

FreeType::FreeType() {
	if (FT_Init_FreeType(&m_lib) != FT_Err_Ok)
		throw std::runtime_error("failed to initalize FreeType");
}

FreeType::~FreeType() {
	if (m_lib)
		FT_Done_FreeType(m_lib);
}

Font FreeType::load(const std::string& file, unsigned int size, unsigned int color, const std::vector<Range>& ranges) {
	FT_Face face;
	
	if (FT_New_Face(m_lib, file.c_str(), 0, &face) != FT_Err_Ok)
		throw std::runtime_error(combine("failed to read font (\"", file, "\")"));

	auto faceFin = finalize(face, FT_Done_Face);

	if (FT_Set_Pixel_Sizes(face, 0, size) != FT_Err_Ok)
		throw std::runtime_error(combine("failed to set size of font (\"", file, "\")"));

	Font f;

	for (auto& range : ranges) {
		for (auto i = range.m_beg; i <= range.m_end; ++i) {
			if (FT_Load_Char(face, i, FT_LOAD_RENDER) != FT_Err_Ok)
				throw std::runtime_error(combine("failed to load character (\"", file, "\")"));

			auto slot = face->glyph;

			f.m_glyphs.push_back({
				i, slot->bitmap_left,
				(int) size - slot->bitmap_top,
				slot->advance.x / 64.0f,
				slot->advance.y / 64.0f,
				imageFromSlot(slot, color)
			});
		}
	}

	return f;
}
#endif
