/*
 * parser.hpp
 *
 *  Created on: 18 мая 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_CONFIG_COMMANDLINE_PARSER_HPP_
#define SRC_CONFIG_COMMANDLINE_PARSER_HPP_

#include "Storage.hpp"

namespace App
{
namespace Config
{

Storage parse_args(int argc, char* argv[]);

} /* namespace Config */
} /* namespace App */

#endif /* SRC_CONFIG_COMMANDLINE_PARSER_HPP_ */
