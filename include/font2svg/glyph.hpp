#pragma once

#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace font2svg {

class glyph {
private:
	FT_GlyphSlot slot;
	FT_Outline ftoutline;
	FT_Glyph_Metrics gm;
	FT_Face face;

  std::string create_outline() const;

public:
  FT_UInt glyph_index;
	FT_ULong charcode;

	FT_Vector* ftpoints;
	char* tags;
	short* contours;

	int bbwidth, bbheight;

	glyph();
	glyph(FT_Face face, FT_UInt glyph_index);
	glyph(FT_Face face, FT_UInt new_glyph_index, FT_ULong new_charcode);

	bool operator==(const glyph& other) const;
	std::string svg() const;
};

}
