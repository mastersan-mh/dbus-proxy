/*
 * server.cpp
 *
 *  Created on: 18 мая 2026 г.
 *      Author: mastersan
 */

#include "server/server.hpp"
#include "logger.hpp"
#include "config_static/Storage.hpp"
#include "helpers/ParseEndpoint.hpp"
#include "helpers/frame.hpp"
#include "helpers/hexprinter.hpp"
#include "helpers/debug.hpp"
#include "utils/io.hpp"

#include <stdexcept>
#include <vector>

#include <string.h>

#include <endian.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sched.h>

namespace App
{
namespace Server
{

static
int P_tcp_listen(const std::string& endpoint)
{
    const struct sockaddr_in addr = GHelpers::parse_endpoint_inet(endpoint);

    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        throw std::runtime_error("socket() failed");
    }

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        throw std::runtime_error("bind() failed");
    }

    if(listen(fd, 128) < 0)
    {
        throw std::runtime_error("listen() failed");
    }

    return fd;
}

static
int unix_connect(const std::string& path)
{
    const int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(fd < 0)
    {
        throw std::runtime_error("socket() failed");
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);

    if(connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        throw std::runtime_error("connect() failed");
    }

    return fd;
}

/**
 * @brief TCP -> Unix: снимаем 4-байтовый префикс, шлём payload как есть
 */
static
void P_relay_tcp_to_unix(
        const Config::Storage& cfg,
        int tcp_fd,
        int unix_fd
)
{
    uint32_t len_be = 0;
    std::vector<uint8_t> payload;
    while(Utils::io::read_exact(tcp_fd, len_be))
    {
        const uint32_t len = be32toh(len_be);
        DEBUG_PRINT("TCP recv {len = %" PRIu32 "}", len);

        if (len == 0 || len > StaticConfig::MAX_FRAME)
        {
            break;
        }

        payload.reserve(len);
        if(!Utils::io::read_exact(tcp_fd, payload.data(), len))
        {
            break;
        }

        DEBUG_CALL(GHelpers::hexprint("TCP -> DBUS: ", payload.data(), len));

        if(!Utils::io::write_exact(unix_fd, payload.data(), len))
        {
            break;
        }
    }
}

/**
 * @brief Unix -> TCP: читаем сырой поток, парсим заголовок D-Bus для границ, добавляем префикс
 */
static
void P_relay_unix_to_tcp(
        const Config::Storage& cfg,
        int dbus_fd,
        int tcp_fd
)
{
    uint8_t tmp[4096];
    GHelpers::WriteBuffer wbuf;

    while(true)
    {
        const ssize_t n = recv(dbus_fd, tmp, sizeof(tmp), 0);
        if (n <= 0)
        {
            break;
        }

        DEBUG_CALL(GHelpers::hexprint("DBUS -> TCP: ", tmp, n));

        GHelpers::Frame::send(cfg, wbuf, tcp_fd, tmp, n);
    }
}

void run(const Config::Storage& cfg)
{
    const int listen_fd = P_tcp_listen(cfg.tcp_endpoint);
    APPLOG_INFO("[Server] Listening on %s", cfg.tcp_endpoint.c_str());

    while (true)
    {
        const int tcp_fd = accept(listen_fd, nullptr, nullptr);
        if (tcp_fd < 0)
        {
            continue;
        }
        const pid_t proc_manager = fork();
        if(proc_manager != 0)
        {
            close(tcp_fd);
            int status;
            waitpid(proc_manager, &status, 0);
        }
        else
        {
            close(listen_fd);
            try
            {
                const int dbus_fd = unix_connect(cfg.dbus_socket);
                const pid_t p1 = fork();
                if (p1 == 0)
                {
                    P_relay_tcp_to_unix(cfg, tcp_fd, dbus_fd);
                    exit(0);
                }

                const pid_t p2 = fork();
                if (p2 == 0)
                {
                    P_relay_unix_to_tcp(cfg, dbus_fd, tcp_fd);
                    exit(0);
                }

                int status;
                const pid_t exited_pid = waitpid(-1, &status, 0);
                if(exited_pid >= 0)
                {
                    const pid_t survived = (p1 == exited_pid ? p2 : p1);
                    kill(survived, SIGTERM);
                    waitpid(survived, nullptr, 0);
                }

                close(dbus_fd);
            }
            catch (const std::exception& e)
            {
                APPLOG_ERROR("[Server child error] %s", e.what());
            }
            exit(0);
        }
    }
}

} /* namespace Server */
} /* namespace App */
