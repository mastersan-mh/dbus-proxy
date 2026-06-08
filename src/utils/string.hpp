/*
 * string.hpp
 *
 *      Author: mastersan
 */

#ifndef SRC_UTILS_STRING_HPP_
#define SRC_UTILS_STRING_HPP_

#include <string>
#include <string_view>
#include <stdint.h>

namespace App
{
namespace Utils
{
namespace String
{

uint64_t str_to_u64(
        std::string_view str
);

} /* namespace String */
} /* namespace Utils */
} /* namespace App */

#endif /* SRC_UTILS_STRING_HPP_ */
