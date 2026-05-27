/*
 * string.hpp
 *
 *  Created on: 27 мая 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_UTILS_STRING_HPP_
#define SRC_UTILS_STRING_HPP_

#include <string>
#include <string_view>
#include <vector>
#include <cctype>

namespace App
{
namespace Utils
{
namespace String
{

std::vector<std::string_view> split(const std::string& str);

} /* namespace String */
} /* namespace Utils */
} /* namespace App */

#endif /* SRC_UTILS_STRING_HPP_ */
