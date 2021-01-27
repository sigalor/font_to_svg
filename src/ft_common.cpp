#include <font2svg/ft_common.hpp>

#include <stdexcept>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace font2svg {

void check_ft_error(const std::string& description, FT_Error code) {
	if(code != 0) throw std::runtime_error(description + ": code " + std::to_string(code));
}

}
