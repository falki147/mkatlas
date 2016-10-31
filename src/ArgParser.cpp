#include "ArgParser.hpp"

#include <memory>
#include <cstring>

#include "Platform.hpp"

inline void errArg(const char* arg) {
	throw std::runtime_error(combine("invalid command line argument: ", arg));
}

Options parseArguments(unsigned int argc, const char** argv) {
	Options opt;

#ifndef DISABLE_FREETYPE
	FontOptions font;
#endif

	for (unsigned int i = 0; i < argc; ++i) {
		if (strncmp(argv[i], "--", 2) == 0) {
			if (strcmp(argv[i] + 2, "expand") == 0) {
				opt.m_expand = true;
			}
			else if (strcmp(argv[i] + 2, "folder") == 0) {
				if (++i >= argc)
					errArg(argv[i - 1]);

				opt.m_outputFolder = argv[i];
			}
			else if (strcmp(argv[i] + 2, "height") == 0) {
				if (++i >= argc)
					errArg(argv[i - 1]);

				opt.m_height = std::stoul(argv[i]);
			}
			else if (strcmp(argv[i] + 2, "help") == 0)
				opt.m_help = true;
			else if (strcmp(argv[i] + 2, "maxtextures") == 0) {
				if (++i >= argc)
					errArg(argv[i - 1]);

				opt.m_maxTextures = std::stoul(argv[i]);
			}
			else if (strcmp(argv[i] + 2, "noflip") == 0)
				opt.m_noFlip = true;
			else if (strcmp(argv[i] + 2, "out") == 0) {
				if (++i >= argc)
					errArg(argv[i - 1]);

				opt.m_output = argv[i];
			}
			else if (strcmp(argv[i] + 2, "padding") == 0) {
				if (++i >= argc)
					errArg(argv[i - 1]);

				opt.m_padding = std::stoul(argv[i]);
			}
			else if (strcmp(argv[i] + 2, "size") == 0) {
				if (++i >= argc)
					errArg(argv[i - 1]);

				opt.m_height = opt.m_width = std::stoul(argv[i]);
			}
			else if (strcmp(argv[i] + 2, "trim") == 0)
				opt.m_trim = true;
			else if (strcmp(argv[i] + 2, "version") == 0)
				opt.m_version = true;
			else if (strcmp(argv[i] + 2, "width") == 0) {
				if (++i >= argc)
					errArg(argv[i - 1]);

				opt.m_width = std::stoul(argv[i]);
			}

		#ifndef DISABLE_FREETYPE
			else if (strcmp(argv[i] + 2, "color") == 0) {
				if (++i >= argc)
					errArg(argv[i - 1]);

				font.m_color = makeRGBFromString(argv[i], 0);
			}
			else if (strcmp(argv[i] + 2, "dfsize") == 0) {
				if (++i >= argc)
					errArg(argv[i - 1]);

				font.m_distantFieldSize = std::stoul(argv[i]);
			}
			else if (strcmp(argv[i] + 2, "dfspread") == 0) {
				if (++i >= argc)
					errArg(argv[i - 1]);

				font.m_distantFieldSpread = std::stoul(argv[i]);
			}
			else if (strcmp(argv[i] + 2, "font") == 0) {
				if (++i >= argc)
					errArg(argv[i - 1]);

				if (!font.m_file.empty() && !font.m_ranges.empty())
					opt.m_fonts.push_back(font);

				font = FontOptions();

				font.m_file = argv[i];
			}
			else if (strcmp(argv[i] + 2, "fontsize") == 0) {
				if (++i >= argc)
					errArg(argv[i - 1]);

				font.m_size = std::stoul(argv[i]);
			}
			else if (strcmp(argv[i] + 2, "name") == 0) {
				if (++i >= argc)
					errArg(argv[i - 1]);

				font.m_name = argv[i];
			}
			else if (strcmp(argv[i] + 2, "range") == 0) {
				if (++i >= argc)
					errArg(argv[i - 1]);

				font.m_ranges.push_back(Range::fromString(argv[i]));
			}
		#endif

			else
				errArg(argv[i]);
		}
		else if (argv[i][0] == '/' || argv[i][0] == '-') {
			if (argv[i][1] != 0 && argv[i][2] == 0) {
				switch (argv[i][1]) {
				case 'e':
				case 'E':
					opt.m_expand = true;
					break;
				case 'h':
				case 'H':
					opt.m_help = true;
					break;
				case 'o':
				case 'O':
					if (++i >= argc)
						errArg(argv[i - 1]);

					opt.m_output = argv[i];
					break;
				case 'p':
				case 'P':
					if (++i >= argc)
						errArg(argv[i - 1]);

					opt.m_padding = std::stoul(argv[i]);
					break;
				case 's':
				case 'S':
					if (++i >= argc)
						errArg(argv[i - 1]);

					opt.m_height = opt.m_width = std::stoul(argv[i]);
					break;
				case 't':
				case 'T':
					opt.m_trim = true;
					break;
				case 'v':
				case 'V':
					opt.m_version = true;
					break;
			#ifndef DISABLE_FREETYPE
				case 'c':
				case 'C':
					if (++i >= argc)
						errArg(argv[i - 1]);

					font.m_color = makeRGBFromString(argv[i], 0);
					break;
				case 'f':
				case 'F':
					if (++i >= argc)
						errArg(argv[i - 1]);

					if (!font.m_file.empty() && !font.m_ranges.empty())
						opt.m_fonts.push_back(font);

					font = FontOptions();

					font.m_file = argv[i];
					break;
				case 'i':
				case 'I':
					if (++i >= argc)
						errArg(argv[i - 1]);

					font.m_size = std::stoul(argv[i]);
					break;
				case 'n':
				case 'N':
					if (++i >= argc)
						errArg(argv[i - 1]);

					font.m_name = argv[i];
					break;
				case 'r':
				case 'R':
					if (++i >= argc)
						errArg(argv[i - 1]);

					font.m_ranges.push_back(Range::fromString(argv[i]));
					break;
			#endif
				default:
					errArg(argv[i]);
				}
			}
			else
				errArg(argv[i]);
		}
		else {
			auto files = glob(argv[i]);
			opt.m_files.insert(opt.m_files.end(), files.begin(), files.end());
		}
	}

#ifndef DISABLE_FREETYPE
	if (!font.m_file.empty())
		opt.m_fonts.push_back(font);

	// Combine and sort ranges
	for (auto& font : opt.m_fonts) {
		std::sort(font.m_ranges.begin(), font.m_ranges.end(), [](auto a, auto b) { return a.m_beg - b.m_beg; });

		for (unsigned int i = 0; !font.m_ranges.empty() && i < font.m_ranges.size() - 1; ++i) {
			if (font.m_ranges[i].isIntersecting(font.m_ranges[i + 1])) {
				font.m_ranges[i] = font.m_ranges[i].join(font.m_ranges[i + 1]);
				font.m_ranges.erase(font.m_ranges.begin() + (i + 1));
			}
		}
	}
#endif

	return opt;
}
