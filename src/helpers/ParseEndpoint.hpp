/*
 * ParseEndpoint.hpp
 *
 *  Created on: 18 мая 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_HELPERS_PARSEENDPOINT_HPP_
#define SRC_HELPERS_PARSEENDPOINT_HPP_

#include <string>
#include <stdexcept>
#include <filesystem>

#include <netinet/in.h>
#include <sys/un.h>

namespace App
{
namespace GHelpers
{

struct sockaddr_in parse_endpoint_inet(const std::string& endpoint);

struct sockaddr_un parse_endpoint_unix(const std::filesystem::path& path);

} /* namespace GHelpers */
} /* namespace App */

#endif /* SRC_HELPERS_PARSEENDPOINT_HPP_ */
