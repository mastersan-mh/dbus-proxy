/*
 * client.h
 *
 *  Created on: 18 мая 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_CLIENT_CLIENT_HPP_
#define SRC_CLIENT_CLIENT_HPP_

#include "config_commandline/Storage.hpp"

namespace App
{
namespace Client
{

void run(const Config::Storage& cfg);

} /* namespace Client */
} /* namespace App */

#endif /* SRC_CLIENT_CLIENT_HPP_ */
