#include "font_to_svg.hpp"

int main(int argc, char** argv) {
	if (argc != 4) {
		std::cerr << "usage: " << argv[0] << " file.ttf [index/codepoint/queryindex/querycodepoint] 42\n";
		exit(1);
	}

	font2svg::ttf_file file(argv[1]);
	std::string operation = std::string(argv[2]);
	int number_param = strtol(argv[3], NULL, 0);

	font2svg::glyph g;
	try {
		if (operation == "index") g = file.glyph_from_index(number_param);
		else if (operation == "codepoint") g = file.glyph_from_codepoint(number_param);
		else if (operation == "queryindex") exit(file.glyph_index_exists(number_param) ? 0 : 1);
		else if (operation == "querycodepoint") exit(file.glyph_codepoint_exists(number_param) ? 0 : 1);
		else {
			std::cerr << "unknown operation: " << operation << std::endl;
			exit(1);
		}
	} catch (const std::runtime_error& e) {
		std::cerr << "error: " << e.what() << std::endl;
		exit(1);
	}
	
	std::cout << g.svgheader() << g.svgtransform() << g.outline() << g.svgfooter();

	file.free();

  return 0;
}
