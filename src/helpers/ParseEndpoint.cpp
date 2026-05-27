/*
 * ParseEndpoint.cpp
 *
 *  Created on: 18 мая 2026 г.
 *      Author: mastersan
 */

#include "helpers/ParseEndpoint.hpp"

#include <arpa/inet.h>
#include <stdint.h>

namespace App
{
namespace GHelpers
{

struct sockaddr_in parse_endpoint_inet(const std::string& endpoint)
{
    const size_t sep = endpoint.rfind(':');
    if (sep == std::string::npos)
    {
        throw std::runtime_error("Invalid endpoint (need host:port)");
    }

    const std::string host = endpoint.substr(0, sep);
    const uint16_t port = std::stoul(endpoint.substr(sep + 1));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htobe16(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    return addr;
}

struct sockaddr_un parse_endpoint_unix(const std::filesystem::path& path)
{
    struct sockaddr_un addr{};
    addr.sun_family = AF_UNIX;

    const std::string path_str = path.string().c_str();
    const char *path_raw = path_str.c_str();
    strncpy(addr.sun_path, path_raw, sizeof(addr.sun_path) - 1);
    return addr;
}

} /* namespace GHelpers */
} /* namespace App */
