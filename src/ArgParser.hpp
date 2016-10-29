#pragma once

#include <string>
#include <vector>

#include "Range.hpp"
#include "Utils.hpp"

#ifndef DISABLE_FREETYPE
struct FontOptions {
	std::string m_file;
	std::string m_name;
	unsigned int m_size = 0;
	unsigned int m_color = makeRBGA(0xFF, 0xFF, 0xFF, 0);
	unsigned int m_distantFieldSize = 1;
	unsigned int m_distantFieldSpread = 0;
	std::vector<Range> m_ranges;
};
#endif

struct Options {
	unsigned int m_maxTextures = 0;
	bool m_help = false;
	bool m_version = false;
	bool m_expand = false;
	bool m_trim = false;
	bool m_noFlip = false;
	unsigned int m_padding = 0;
	unsigned int m_width = 1024;
	unsigned int m_height = 1024;
	std::string m_outputFolder;
	std::string m_output = "atlas.json";
	std::vector<std::string> m_files;

#ifndef DISABLE_FREETYPE
	std::vector<FontOptions> m_fonts;
#endif
};

Options parseArguments(unsigned int argc, const char** argv);
