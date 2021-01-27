# font_to_svg

This code will convert a single character in a TrueType(R) font file into an SVG 'path' shape and can also reconstruct missing character maps from embedded PDF fonts. This repository is a fork of [the one from donbright](https://github.com/donbright/font_to_svg), but with a lot of code removed to be more minimalistic. Additionally, this code has no debug output and the resulting SVG files will have no comments.

## Building

Just run `make`. The development headers for FreeType 2 need to be installed on your system.

## Usage

In general, in TrueType fonts, there two different methods of accessing glyphs: either using the actual glyph index or by first mapping a Unicode codepoint to such a glyph index (using FreeType's [FT_Get_Char_Index](https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_get_char_index)).

### extract_svg

The `extract_svg` executable is used like this:

```
bin/extract_svg file.ttf [index/codepoint/queryindex/querycodepoint] 42
```

It offers four different operations:

1. `index`: Prints the SVG representation of the glyph at the given index in the given TTF file.
2. `codepoint`: Prints the SVG representation of the glyph with the given Unicode codepoint.
3. `queryindex`: Exit with code 0 if the given glyph index exists in the TTF file, and with code 1 otherwise.
4. `querycodepoint`: Exit with code 0 if a glyph with the given codepoint exists in the TTF file, and with code 1 otherwise.

### build_charmap

When fonts are embedded into a PDF, often only the needed set of characters are included. However, in the worst case, the glyphs of those embedded fonts are not associated with Unicode codepoints anymore, which makes it impossible to directly read out text from the PDF, even though a human can still read the PDF normally (also see [here](https://filingdb.com/b/pdf-text-extraction), section _Embedded Fonts_).

If you know which font to expect, you are in luck though. For instance, if the embedded font contains just the glyph of the letter 'a' in the Arial font, but no Unicode codepoint associated with it, you can look at the glyphs of the original Arial font and find the glyph which is exactly the same as the one in the embedded font. This is exactly what `build_charmap` does, but note that it can have quadratic time complexity, because each input glyph needs to be compared against each reference glyph.

For example, run it like this, where the input TTF has been extracted from a PDF file using [`mutool extract`](https://mupdf.com/docs/manual-mutool-extract.html) (also see [here](https://tex.stackexchange.com/a/156438) for why there are always a six uppercase letters in front of the filename) and the reference TTF is the regular Arial font (e.g. from [here](http://corefonts.sourceforge.net) or its [Debian package](https://packages.debian.org/en/stretch/msttcorefonts)):

```
bin/build_charmap CGIBBM+ArialMT-0066.ttf /usr/share/fonts/truetype/msttcorefonts/Arial.ttf
```

## Trademark disclaimer

TrueType is a trademark of Apple, Inc. This library is not in any way
affiliated nor endorsed by Apple, Inc.
