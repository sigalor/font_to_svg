font_to_svg: font_to_svg.cpp
	g++ -I/usr/include/freetype2 -g -o $@ $< -lfreetype
