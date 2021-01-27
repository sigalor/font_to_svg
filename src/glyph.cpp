#include <font2svg/glyph.hpp>

#include <sstream>
#include <font2svg/ft_common.hpp>

namespace font2svg {

/* Draw the outline of the font as svg.
There are three main components.
1. the points
2. the 'tags' for the points
3. the contour indexes (that define which points belong to which contour)
*/
std::string glyph::create_outline() const {
	std::stringstream svg;
	if (points.size() == 0) return "";
	if (contours.size() == 0) return "";
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
	for (size_t i = 0; i < contours.size(); i++) {
		contour_endi = contours[i];
		int offset = contour_starti;
		int npts = contour_endi - contour_starti + 1;
		svg << " M " << points[contour_starti].x << "," << points[contour_starti].y;
		for (int j = 0; j < npts; j++) {
			int thisi = j%npts + offset;
			int nexti = (j+1)%npts + offset;
			int nextnexti = (j+2)%npts + offset;
			int x = points[thisi].x;
			int y = points[thisi].y;
			int nx = points[nexti].x;
			int ny = points[nexti].y;
			int nnx = points[nextnexti].x;
			int nny = points[nextnexti].y;
			bool this_tagbit1 = (tags[thisi] & 1);
			bool next_tagbit1 = (tags[nexti] & 1);
			bool nextnext_tagbit1 = (tags[nextnexti] & 1);
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

glyph::glyph() {}

glyph::glyph(FT_Face face, FT_UInt glyph_index) : glyph(face, glyph_index, 0) {}

glyph::glyph(FT_Face face, FT_UInt new_glyph_index, FT_ULong new_charcode) : glyph_index(new_glyph_index), charcode(new_charcode) {
  check_ft_error("FT_Load_Glyph", FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_SCALE));
  slot = face->glyph;
  ftoutline = slot->outline;
  gm = slot->metrics;

	points = std::vector<FT_Vector>(ftoutline.points, ftoutline.points+ftoutline.n_points);
	tags = std::vector<char>(ftoutline.tags, ftoutline.tags + ftoutline.n_points);
	contours = std::vector<short>(ftoutline.contours, ftoutline.contours + ftoutline.n_contours);
	bbheight = face->bbox.yMax - face->bbox.yMin;
  bbwidth = face->bbox.xMax - face->bbox.xMin;

  // Invert y coordinates (SVG = neg at top, TType = neg at bottom)
	for (FT_Vector& point : points) {
		point.y *= -1;
	}
}

bool glyph::operator==(const glyph& other) const {
  if (points.size() != other.points.size())
    return false;
  if (contours.size() != other.contours.size())
    return false;

  for (size_t i = 0; i < points.size(); i++) {
    if (points[i].x != other.points[i].x || points[i].y != other.points[i].y)
      return false;
    if (tags[i] != other.tags[i])
      return false;
  }

  for (size_t i = 0; i < contours.size(); i++) {
    if (contours[i] != other.contours[i])
      return false;
  }

  return true;
}

std::string glyph::svg() const {
  std::stringstream ret;

  // header, possibly also use   width='" << bbwidth << "px'" << " height='" << bbheight << "px'"
  ret << "<svg xmlns='http://www.w3.org/2000/svg' version='1.1'>";

  // TrueType points are not in the range usually visible by SVG.
  // they often have negative numbers etc. So, here we
  // 'transform' to make visible.
  // note also that y coords of all points have been flipped during
  // init() so that SVG Y positive = Truetype Y positive
  int yadj = gm.horiBearingY + gm.vertBearingY + 100;
  ret << "<g fill-rule='nonzero' transform='translate(" << 100 << " " << yadj << ")'>";
  
  // outline and footer
  ret << create_outline();
  ret << "</g></svg>";

  return ret.str();
}

}
