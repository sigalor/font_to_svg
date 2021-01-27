#include <iostream>

#include <font2svg/ttf_file.hpp>
#include <font2svg/glyph.hpp>

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "usage: " << argv[1] << " [input ttf] [reference ttf]" << std::endl;
    exit(1);
  }

  FT_Library library;
  FT_Init_FreeType(&library);
  font2svg::ttf_file input(library, argv[1]);
  font2svg::ttf_file reference(library, argv[2]);

  std::vector<font2svg::glyph> input_glyphs = input.get_all_glyphs();
  for(const font2svg::glyph& g : input_glyphs) {
    std::cout << g.svg() << std::endl;
  }

  input.free();
  reference.free();
  FT_Done_FreeType(library);

  return 0;
}
