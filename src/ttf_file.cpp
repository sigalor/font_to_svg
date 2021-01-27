#include <font2svg/ttf_file.hpp>

#include <iostream>
#include <stdexcept>
#include <font2svg/ft_common.hpp>

namespace font2svg {

ttf_file::ttf_file(FT_Library new_library, const std::string& filename) : library(new_library) {
  check_ft_error("FT_New_Face", FT_New_Face(library, filename.c_str(), 0, &face));
}

void ttf_file::free() {
  check_ft_error("FT_Done_Face", FT_Done_Face(face));
}

glyph ttf_file::glyph_from_index(FT_UInt glyph_index) {
  return glyph(face, glyph_index);
}

glyph ttf_file::glyph_from_codepoint(int codepoint) {
  FT_UInt glyph_index = FT_Get_Char_Index(face, codepoint);
  if(glyph_index == 0) throw std::runtime_error("unknown codepoint: " + std::to_string(codepoint));
  return glyph(face, glyph_index);
}

bool ttf_file::glyph_codepoint_exists(int codepoint) {
  return FT_Get_Char_Index(face, codepoint) != 0;
}

bool ttf_file::glyph_index_exists(FT_UInt glyph_index) {
  return FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_SCALE) == 0;
}

std::vector<glyph> ttf_file::get_all_glyphs() {
  std::vector<glyph> ret;
  FT_UInt glyph_index;
  FT_ULong charcode = FT_Get_First_Char(face, &glyph_index);

  if (glyph_index == 0) {
    // if the first glyph index is 0 already, try to query for glyphs anyway
    for (FT_UInt i = 0; i < 0x10000; i++) {
      if(glyph_index_exists(i))
        ret.push_back(glyph(face, i));
    }
  } else {
    while (glyph_index != 0) {
      ret.push_back(glyph(face, glyph_index, charcode));
      charcode = FT_Get_Next_Char(face, charcode, &glyph_index);
    }
  }

  return ret;
}

}
