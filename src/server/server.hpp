/*
 * server.hpp
 *
 *  Created on: 18 мая 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_SERVER_SERVER_HPP_
#define SRC_SERVER_SERVER_HPP_

#include "config_commandline/Storage.hpp"

namespace App
{
namespace Server
{

void run(const Config::Storage& cfg);

} /* namespace Server */
} /* namespace App */

#endif /* SRC_SERVER_SERVER_HPP_ */
