#include <iostream>

#include <font2svg/ttf_file.hpp>
#include <font2svg/glyph.hpp>

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "usage: " << argv[0] << " [input ttf] [reference ttf]" << std::endl;
    exit(1);
  }

  FT_Library library;
  FT_Init_FreeType(&library);
  font2svg::ttf_file input(library, argv[1]);
  font2svg::ttf_file reference(library, argv[2]);

  std::vector<font2svg::glyph> input_glyphs = input.get_all_glyphs();
  std::vector<font2svg::glyph> reference_glyphs = reference.get_all_glyphs();

  // find input glyphs in set of reference glyphs
  for (const font2svg::glyph& input_glyph : input_glyphs) {
    for (const font2svg::glyph& reference_glyph : reference_glyphs) {
      if(input_glyph == reference_glyph) {
        std::cout << "input glyph index " << input_glyph.glyph_index << " --> codepoint " << reference_glyph.charcode << std::endl;
        break;
      }
    }
  }

  input.free();
  reference.free();
  FT_Done_FreeType(library);

  return 0;
}
