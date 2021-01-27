// font_to_svg.hpp - Read Font in TrueType (R) format, write SVG
// Copyright Don Bright 2013 <hugh.m.bright@gmail.com>
/*

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  License based on zlib license, by Jean-loup Gailly and Mark Adler
*/

/*

This program reads a TTF (TrueType (R)) file and outputs an SVG path.

See these sites for more info.
Basic Terms: http://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html
FType + outlines: http://www.freetype.org/freetype2/docs/reference/ft2-outline_processinhtml
FType + contours: http://www.freetype.org/freetype2/docs/glyphs/glyphs-6.html
TType contours: https://developer.apple.com/fonts/TTRefMan/RM01/Chap1.html
TType contours2: http://www.truetype-typography.com/ttoutln.htm
Non-zero winding rule: http://en.wikipedia.org/wiki/Nonzero-rule
SVG paths: http://www.w3schools.com/svg/svg_path.asp
SVG paths + nonzero: http://www.w3.org/TR/SVG/paintinhtml#FillProperties

TrueType is a trademark of Apple Inc. Use of this mark does not imply
endorsement.

*/

#ifndef __font_to_svg_h__
#define __font_to_svg_h__

#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

namespace font2svg {

void check_ft_error(const std::string& description, FT_Error code) {
	if(code != 0) throw std::runtime_error(description + ": code " + std::to_string(code));
}

/* Draw the outline of the font as svg.
There are three main components.
1. the points
2. the 'tags' for the points
3. the contour indexes (that define which points belong to which contour)
*/
std::string do_outline(std::vector<FT_Vector> points, std::vector<char> tags, std::vector<short> contours)
{
	std::stringstream svg;
	if (points.size()==0) return "";
	if (contours.size()==0) return "";
	svg	<< "<path fill='black' d='";

	/* tag bit 1 indicates whether its a control point on a bez curve
	or not. two consecutive control points imply another point halfway
	between them */

	// Step 1. move to starting point (M x-coord y-coord )
	// Step 2. decide whether to draw a line or a bezier curve or to move
	// Step 3. for bezier: Q control-point-x control-point-y,
	//		         destination-x, destination-y
	//         for line:   L x-coord, y-coord
	//         for move:   M x-coord, y-coord

	int contour_starti = 0;
	int contour_endi = 0;
	for ( int i = 0 ; i < contours.size() ; i++ ) {
		contour_endi = contours.at(i);
		int offset = contour_starti;
		int npts = contour_endi - contour_starti + 1;
		svg << " M " << points[contour_starti].x << "," << points[contour_starti].y;
		for ( int j = 0; j < npts; j++ ) {
			int thisi = j%npts + offset;
			int nexti = (j+1)%npts + offset;
			int nextnexti = (j+2)%npts + offset;
			int x = points[thisi].x;
			int y = points[thisi].y;
			int nx = points[nexti].x;
			int ny = points[nexti].y;
			int nnx = points[nextnexti].x;
			int nny = points[nextnexti].y;
			bool this_tagbit1 = (tags[ thisi ] & 1);
			bool next_tagbit1 = (tags[ nexti ] & 1);
			bool nextnext_tagbit1 = (tags[ nextnexti ] & 1);
			bool this_isctl = !this_tagbit1;
			bool next_isctl = !next_tagbit1;
			bool nextnext_isctl = !nextnext_tagbit1;

			if (this_isctl && next_isctl) {
				x = (x + nx) / 2;
				y = (y + ny) / 2;
				this_isctl = false;
				if (j==0) {
					svg << " M " << x << "," << y;
				}
			}

			if (!this_isctl && next_isctl && !nextnext_isctl) {
				svg << " Q " << nx << "," << ny << " " << nnx << "," << nny;
			} else if (!this_isctl && next_isctl && nextnext_isctl) {
				nnx = (nx + nnx) / 2;
				nny = (ny + nny) / 2;
				svg << " Q " << nx << "," << ny << " " << nnx << "," << nny;
			} else if (!this_isctl && !next_isctl) {
				svg << " L " << nx << "," << ny;	
			}
		}
		contour_starti = contour_endi+1;
		svg << " Z";
	}
	svg << "'/>";
	return svg.str();
}

class glyph {
public:
	FT_UInt glyphIndex;
	FT_GlyphSlot slot;
	FT_Outline ftoutline;
	FT_Glyph_Metrics gm;
	FT_Face face;

	FT_Vector* ftpoints;
	char* tags;
	short* contours;

	int bbwidth, bbheight;

	glyph() {}

	glyph(FT_Face face, FT_UInt glyph_index) {
		check_ft_error("FT_Load_Glyph", FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_SCALE));
		slot = face->glyph;
		ftoutline = slot->outline;
		gm = slot->metrics;

		// Invert y coordinates (SVG = neg at top, TType = neg at bottom)
		ftpoints = ftoutline.points;
		for ( int i = 0 ; i < ftoutline.n_points ; i++ )
			ftpoints[i].y *= -1;

		bbheight = face->bbox.yMax - face->bbox.yMin;
		bbwidth = face->bbox.xMax - face->bbox.xMin;
		tags = ftoutline.tags;
		contours = ftoutline.contours;
	}

	std::string svgheader() {
		std::stringstream header;

		/*header << "<svg width='" << bbwidth << "px'"
			<< " height='" << bbheight << "px'"
			<< " xmlns='http://www.w3.org/2000/svg' version='1.1'>";*/
		
		header << "<svg xmlns='http://www.w3.org/2000/svg' version='1.1'>";

		return header.str();
	}

	std::string svgtransform() {
		// TrueType points are not in the range usually visible by SVG.
		// they often have negative numbers etc. So.. here we
		// 'transform' to make visible.
		//
		// note also that y coords of all points have been flipped during
		// init() so that SVG Y positive = Truetype Y positive
		std::stringstream transform;

		int yadj = gm.horiBearingY + gm.vertBearingY + 100;
		int xadj = 100;
		transform << "<g fill-rule='nonzero' "
			<< "transform='translate(" << xadj << " " << yadj << ")'"
			<< ">";
		return transform.str();
	}

	std::string outline()  {
		std::vector<FT_Vector> pointsv(ftpoints,ftpoints+ftoutline.n_points);
		std::vector<char> tagsv(tags,tags+ftoutline.n_points);
		std::vector<short> contoursv(contours,contours+ftoutline.n_contours);
		return do_outline(pointsv, tagsv, contoursv);
	}

	std::string svgfooter()  {
		return "</g></svg>";
	}
};

class ttf_file {
public:
	FT_Library library;
	FT_Face face;

	ttf_file(const std::string& filename) {
		check_ft_error("FT_Init_FreeType", FT_Init_FreeType(&library));
		check_ft_error("FT_New_Face", FT_New_Face(library, filename.c_str(), 0, &face));
	}

	void free() {
		check_ft_error("FT_Done_Face", FT_Done_Face( face ));
		check_ft_error("FT_Done_FreeType", FT_Done_FreeType( library ));
	}

	glyph glyph_from_index(FT_UInt glyph_index) {
		return glyph(face, glyph_index);
	}

	glyph glyph_from_codepoint(int codepoint) {
		FT_UInt glyph_index = FT_Get_Char_Index(face, codepoint);
		if(glyph_index == 0) throw std::runtime_error("unknown codepoint: " + std::to_string(codepoint));
		return glyph(face, glyph_index);
	}

	bool glyph_codepoint_exists(int codepoint) {
		return FT_Get_Char_Index(face, codepoint) != 0;
	}

	bool glyph_index_exists(FT_UInt glyph_index) {
		return FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_SCALE) == 0;
	}
};

} // namespace

#endif

