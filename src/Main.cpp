#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "ArgParser.hpp"
#include "Image.hpp"
#include "Platform.hpp"
#include "MaxRects.hpp"
#include "Canvas.hpp"
#include "JSONWriter.hpp"
#include "DistantField.hpp"

#ifndef DISABLE_FREETYPE
	#include "Font.hpp"
#endif

const char versionString[] =
	"MKATLAS v1.0\n"
#ifdef DISABLE_FREETYPE
	"Compiled on " __TIMESTAMP__ " without FreeType support\n"
#else
	"Compiled on " __TIMESTAMP__ " with FreeType support\n"
#endif
	"Written by Florian Preinfalk\n"
	"\n"
	"The bin packing algorithm is based on Jukka Jylanki's \"A Thousand Ways to Pack the Bin\"\n"
	"\n"
	"Libraries used:\n"
	"\tlibpng\n"
#ifndef DISABLE_FREETYPE
	"\tFreeType\n"
#endif
	;

const char helpString[] =
#ifdef DISABLE_FREETYPE
	"Generate a texture atlas from images.\n"
#else
	"Generate a texture atlas from images or fonts.\n"
#endif
	"\n"
	"Usage: mkatlas [options] files\n"
	"\n"
	"Options:\n"
	"\t-h --help           Show this screen.\n"
	"\t-v --version        Show version.\n"
	"\t--maxTextures <val> Set texture limit.\n"
	"\t-e --expand         Expand borders of images. Depends on padding.\n"
	"\t-t --trim           Remove transparent borders of images.\n"
	"\t--noflip            Disable rotation of images.\n"
	"\t-p --padding <val>  Set padding between images.\n"
	"\t--width <width>     Set width of textures.\n"
	"\t--height <height>   Set height of textures.\n"
	"\t-s --size <size>    Set width and height of textures.\n"
	"\t--folder <folder>   Set output folder of textures.\n"
	"\t-o --out <output>   Set output file.\n"
#ifndef DISABLE_FREETYPE
	"\t-f --font <file>    Add font.\n"
	"\t-n --name <name>    Set name of font.\n"
	"\t-i --size <size>    Set size of font.\n"
	"\t-c --color <color>  Set color of font in hex.\n"
	"\t--dfsize <size>     Set scaling of input image used to generate signed distant field.\n"
	"\t--dfspread <spread> Set spread of signed distant field.\n"
	"\t-r --range <range>  Add characters to font (can be <num> or <beg>-<end>).\n"
#endif
	;

int main(int argc, const char** argv) {
	try {
	#ifndef DISABLE_FREETYPE
		FreeType ft;
	#endif

		auto opt = parseArguments((unsigned int) argc - 1, argv + 1);

		if (opt.m_help) {
			std::cout << helpString << std::endl;
			return 0;
		}

		if (opt.m_version) {
			std::cout << versionString << std::endl;
			return 0;
		}

	#ifdef DISABLE_FREETYPE
		if (opt.m_files.empty())
			throw std::runtime_error("no input files selected");
	#else
		if (opt.m_files.empty() && opt.m_fonts.empty())
			throw std::runtime_error("no input files selected");
	#endif

		// Parse filenames
		std::vector<std::string> imageNames;
		imageNames.reserve(opt.m_files.size());

		for (auto& file : opt.m_files)
			imageNames.push_back(stripExtension(stripBase(file)));

		// Load images
		std::vector<Image> images;
		images.reserve(opt.m_files.size());

		for (auto& file : opt.m_files)
			images.push_back(Image::load(file));

		std::vector<Rectangle> imageBounds;

		if (opt.m_trim) {
			imageBounds.reserve(images.size());

			for (auto& img : images)
				imageBounds.push_back(img.getBounds());
		}

	#ifndef DISABLE_FREETYPE
		// Load fonts
		std::vector<Font> fonts;
		fonts.reserve(opt.m_fonts.size());

		for (auto& font : opt.m_fonts)
			fonts.push_back(ft.load(font.m_file, font.m_size * font.m_distantFieldSize, font.m_color, font.m_ranges));

		for (unsigned int i = 0; i < fonts.size(); ++i)
			if (opt.m_fonts[i].m_distantFieldSpread != 0)
				for (unsigned int j = 0; j < fonts[i].m_glyphs.size(); ++j)
					fonts[i].m_glyphs[j].m_img = distantFieldFromImage(
						fonts[i].m_glyphs[j].m_img, opt.m_fonts[i].m_distantFieldSpread, opt.m_fonts[i].m_distantFieldSize
					);
	#endif

		MaxRects mr({
			opt.m_expand ? opt.m_width : opt.m_width + opt.m_padding,
			opt.m_expand ? opt.m_height : opt.m_height + opt.m_padding,
			opt.m_maxTextures, !opt.m_noFlip
		});

		// Add images to rectangle packer
		std::vector<RectData> imageRects(images.size());

		for (unsigned int i = 0; i < images.size(); ++i)
			if (opt.m_trim)
				mr.add(&imageRects[i], imageBounds[i].m_w + opt.m_padding, imageBounds[i].m_h + opt.m_padding);
			else
				mr.add(&imageRects[i], images[i].width() + opt.m_padding, images[i].height() + opt.m_padding);

	#ifndef DISABLE_FREETYPE
		// Add glyphs to rectangle packer
		std::vector<std::vector<RectData>> fontRects;
		fontRects.reserve(fonts.size());

		for (auto& font : fonts) {
			fontRects.emplace_back(font.m_glyphs.size());

			for (unsigned int i = 0; i < font.m_glyphs.size(); ++i)
				mr.add(&fontRects.back()[i], font.m_glyphs[i].m_img.width() + opt.m_padding, font.m_glyphs[i].m_img.height() + opt.m_padding);
		}
	#endif

		if (!mr.pack())
			throw std::runtime_error("failed to pack rectangles");

		std::vector<Canvas> canvases(mr.getNumBins(), Canvas(opt.m_width, opt.m_height));

		for (unsigned int i = 0; i < images.size(); ++i)
			if (opt.m_trim)
				canvases[imageRects[i].m_bin].drawRect(images[i], imageBounds[i], imageRects[i].m_x, imageRects[i].m_y, imageRects[i].m_flipped, opt.m_expand ? opt.m_padding : 0);
			else
				canvases[imageRects[i].m_bin].draw(images[i], imageRects[i].m_x, imageRects[i].m_y, imageRects[i].m_flipped, opt.m_expand ? opt.m_padding : 0);

	#ifndef DISABLE_FREETYPE
		for (unsigned int i = 0; i < fonts.size(); ++i)
			for (unsigned int j = 0; j < fonts[i].m_glyphs.size(); ++j)
				canvases[fontRects[i][j].m_bin].draw(fonts[i].m_glyphs[j].m_img, fontRects[i][j].m_x, fontRects[i][j].m_y, fontRects[i][j].m_flipped, opt.m_expand ? opt.m_padding : 0);
	#endif

		for (unsigned int i = 0; i < canvases.size(); ++i) {
			std::stringstream ss;
			ss << "texture" << std::setw(2) << std::setfill('0') << i << ".png";
			canvases[i].getImage().save(ss.str());
		}

		// Write to JSON file
		std::ofstream f(opt.m_output);

		JSONWriter writer(f);

		writer.begin();

		writer.key("textures");
		writer.beginArray();

		for (unsigned int i = 0; i < mr.getNumBins(); ++i) {
			writer.begin();

			writer.key("file");
			std::stringstream ss;
			ss << "texture" << std::setw(2) << std::setfill('0') << i << ".png";
			canvases[i].getImage().save(ss.str());
			writer.writeString(ss.str());

			writer.key("width");
			writer.writeUint(canvases[i].getImage().width());

			writer.key("height");
			writer.writeUint(canvases[i].getImage().height());

			writer.end();
		}

		writer.end();

		if (!images.empty()) {
			writer.key("images");
			writer.beginArray();

			for (unsigned int i = 0; i < images.size(); ++i) {
				writer.begin();

				if (!images[i].empty() && (!opt.m_trim || imageBounds[i].m_w != 0 || imageBounds[i].m_h != 0)) {
					writer.key("texture");
					writer.writeUint(imageRects[i].m_bin);

					writer.key("name");
					writer.writeString(imageNames[i]);

					if (opt.m_trim && (imageBounds[i].m_x != 0 || imageBounds[i].m_y != 0 ||
						imageBounds[i].m_w != images[i].width() || imageBounds[i].m_h != images[i].height())) {

						writer.key("offsetX");
						writer.writeUint(imageBounds[i].m_x);

						writer.key("offsetY");
						writer.writeUint(imageBounds[i].m_y);

						writer.key("realWidth");
						writer.writeUint(images[i].width());

						writer.key("realHeight");
						writer.writeUint(images[i].height());
					}

					writer.key("x");
					writer.writeUint(imageRects[i].m_x);

					writer.key("y");
					writer.writeUint(imageRects[i].m_y);

					writer.key("width");
					writer.writeUint(opt.m_trim ? imageBounds[i].m_w : images[i].width());

					writer.key("height");
					writer.writeUint(opt.m_trim ? imageBounds[i].m_h : images[i].height());

					if (!opt.m_noFlip) {
						writer.key("flipped");
						writer.writeBool(imageRects[i].m_flipped);
					}
				}
				else {
					writer.key("name");
					writer.writeString(imageNames[i]);

					writer.key("realWidth");
					writer.writeUint(images[i].width());

					writer.key("realHeight");
					writer.writeUint(images[i].height());
				}

				writer.end();
			}

			writer.end();
		}

	#ifndef DISABLE_FREETYPE
		if (!fonts.empty()) {
			writer.key("fonts");
			writer.beginArray();

			for (unsigned int i = 0; i < fonts.size(); ++i) {
				writer.begin();

				writer.key("name");
				writer.writeString(opt.m_fonts[i].m_name);

				writer.key("size");
				writer.writeUint(opt.m_fonts[i].m_size);

				if (!fonts[i].m_glyphs.empty()) {
					writer.key("glyphs");
					writer.beginArray();

					for (unsigned int j = 0; j < fonts[i].m_glyphs.size(); ++j) {
						writer.begin();
						
						if (!fonts[i].m_glyphs[j].m_img.empty()) {
							writer.key("texture");
							writer.writeUint(fontRects[i][j].m_bin);

							writer.key("code");
							writer.writeUint(fonts[i].m_glyphs[j].m_ind);

							writer.key("x");
							writer.writeUint(fontRects[i][j].m_x);

							writer.key("y");
							writer.writeUint(fontRects[i][j].m_y);

							writer.key("width");
							writer.writeUint(fonts[i].m_glyphs[j].m_img.width());

							writer.key("height");
							writer.writeUint(fonts[i].m_glyphs[j].m_img.height());

							if (!opt.m_noFlip) {
								writer.key("flipped");
								writer.writeBool(fontRects[i][j].m_flipped);
							}

							writer.key("advX");
							writer.writeDouble(fonts[i].m_glyphs[j].advX);

							writer.key("advY");
							writer.writeDouble(fonts[i].m_glyphs[j].advY);

							writer.key("transX");
							writer.writeInt(fonts[i].m_glyphs[j].transX);

							writer.key("transY");
							writer.writeInt(fonts[i].m_glyphs[j].transY);
						}
						else {
							writer.key("code");
							writer.writeUint(fonts[i].m_glyphs[j].m_ind);

							writer.key("advX");
							writer.writeDouble(fonts[i].m_glyphs[j].advX);

							writer.key("advY");
							writer.writeDouble(fonts[i].m_glyphs[j].advY);
						}

						writer.end();
					}

					writer.end();
				}

				writer.end();
			}

			writer.end();
		}
	#endif

		writer.end();
		f << '\n';
	}
	catch (std::exception& ex) {
		std::cout << "error: " << ex.what() << std::endl;
		return 1;
	}

	return 0;
}
