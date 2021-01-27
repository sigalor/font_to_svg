# font_to_svg

This code will convert a single character in a TrueType(R) font file into an SVG 'path' shape. This repository is a fork of [the one from donbright](https://github.com/donbright/font_to_svg), but with a lot of code removed to be more minimalistic. Additionally, this code has no debug output and the resulting SVG files will have no comments.

## Building

Just run `make`. The development headers for FreeType 2 need to be installed on your system.

## Usage

In general, in TrueType fonts, there two different methods of accessing glyphs: either using the actual glyph index or by first mapping a Unicode codepoint to such a glyph index (using FreeType's [FT_Get_Char_Index](https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_get_char_index)).

In general, the `font_to_svg` executable is used like this:

```
./font_to_svg file.ttf [index/codepoint/queryindex/querycodepoint] 42
```

It offers four different operations:

1. `index`: Prints the SVG representation of the glyph at the given index in the given TTF file.
2. `codepoint`: Prints the SVG representation of the glyph with the given Unicode codepoint.
3. `queryindex`: Exit with code 0 if the given glyph index exists in the TTF file, and with code 1 otherwise.
4. `querycodepoint`: Exit with code 0 if a glyph with the given codepoint exists in the TTF file, and with code 1 otherwise.

### Trademark disclaimer

TrueType is a trademark of Apple, Inc. This library is not in any way
affiliated nor endorsed by Apple, Inc.
