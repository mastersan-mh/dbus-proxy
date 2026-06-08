/*
 * string.cpp
 *
 *      Author: mastersan
 */

#include "utils/string.hpp"

#include <stdexcept>
#include <charconv>
#include <ctype.h>

namespace App
{
namespace Utils
{
namespace String
{

uint64_t str_to_u64(
        std::string_view str
)
{
     uint64_t res = 0;
     auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), res);
     if(ec != std::errc{} || ptr != str.data() + str.size())
     {
         throw std::invalid_argument("Invalid value");
     }
     return res;
}

} /* namespace String */
} /* namespace Utils */
} /* namespace App */
