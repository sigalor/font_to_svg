#pragma once

#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace font2svg {

void check_ft_error(const std::string& description, FT_Error code);

}
