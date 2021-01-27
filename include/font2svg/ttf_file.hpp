#pragma once

#include <string>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "glyph.hpp"

namespace font2svg {

class ttf_file {
public:
	FT_Library library;
	FT_Face face;

	ttf_file(FT_Library new_library, const std::string& filename);
	void free();

	glyph glyph_from_index(FT_UInt glyph_index);
	glyph glyph_from_codepoint(int codepoint);
	bool glyph_codepoint_exists(int codepoint);
	bool glyph_index_exists(FT_UInt glyph_index);
	std::vector<glyph> get_all_glyphs();
};

}
