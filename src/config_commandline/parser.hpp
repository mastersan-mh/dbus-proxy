/*
 * parser.hpp
 *
 *  Created on: 18 may 2026 г.
 *      Author: mastersan
 */

#pragma once

#include "config_storage/Storage.hpp"

namespace App
{
namespace Config
{

Storage parse_args(int argc, char* argv[]);

} /* namespace Config */
} /* namespace App */

