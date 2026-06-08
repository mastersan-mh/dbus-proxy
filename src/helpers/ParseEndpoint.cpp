/*
 * ParseEndpoint.cpp
 *
 *  Created on: 18 мая 2026 г.
 *      Author: mastersan
 */

#include "helpers/ParseEndpoint.hpp"

#include <stdexcept>
#include <arpa/inet.h>
#include <stdint.h>

namespace App
{
namespace GHelpers
{

struct sockaddr_in build_sockaddr_in(
        const std::string& addr,
        uint16_t port
)
{
    sockaddr_in addr_in{};
    addr_in.sin_family = AF_INET;

    addr_in.sin_port = htobe16(port);
    const char *addr_raw = addr.c_str();
    inet_pton(AF_INET, addr_raw, &addr_in.sin_addr);

    return addr_in;
}

struct sockaddr_un build_sockaddr_un(
        const std::filesystem::path& path
)
{
    struct sockaddr_un addr_un{};
    addr_un.sun_family = AF_UNIX;

    static const size_t max_len = sizeof(addr_un.sun_path) - 1;
    if(path.string().length() > max_len)
    {
        throw std::overflow_error("Too long unix socket path");
    }

    const char *path_raw = path.c_str();
    strncpy(addr_un.sun_path, path_raw, max_len);
    return addr_un;
}

} /* namespace GHelpers */
} /* namespace App */
