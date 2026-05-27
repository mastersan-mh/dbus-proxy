/*
 * client.cpp
 *
 *  Created on: 18 мая 2026 г.
 *      Author: mastersan
 */

#include "client/client.hpp"
#include "utils/io.hpp"
#include "logger.hpp"
#include "config_static/Storage.hpp"
#include "helpers/ParseEndpoint.hpp"
#include "helpers/hexprinter.hpp"
#include "helpers/frame.hpp"
#include "helpers/debug.hpp"

#include <stdexcept>
#include <vector>
#include <filesystem>

#include <string.h>

#include <endian.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/prctl.h>

namespace App
{
namespace Client
{

static
int P_tcp_connect(const std::string& endpoint)
{
    const struct sockaddr_in addr = GHelpers::parse_endpoint_inet(endpoint);

    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        throw std::runtime_error("socket() failed");
    }

    if(connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        throw std::runtime_error("connect() failed");
    }

    return fd;
}

static
int P_unix_listen(const std::filesystem::path& path)
{
    const struct sockaddr_un addr = GHelpers::parse_endpoint_unix(path);

    const int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
    {
        throw std::runtime_error("socket() failed");
    }

    unlink(addr.sun_path);
    if(bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        throw std::runtime_error("bind() failed");
    }

    std::filesystem::permissions(
            path,
            std::filesystem::perms::all,
            std::filesystem::perm_options::replace
    );

    if (listen(fd, 16) < 0)
    {
        throw std::runtime_error("listen() failed");
    }

    return fd;
}

/**
 * @brief Клиент: подмена AUTH + relay
 */
static
void P_relay_unix_to_tcp(
        const Config::Storage& cfg,
        int dbus_fd,
        int tcp_fd
)
{
    GHelpers::WriteBuffer wbuf;

    static const size_t buf_capacity = 256;
    uint8_t buf[buf_capacity];

    while(true)
    {
        const ssize_t buf_size = recv(dbus_fd, buf, sizeof(buf), 0);
        if(buf_size <= 0)
        {
            return;
        }

        DEBUG_CALL(GHelpers::hexprint("DBUS -> TCP: ", buf, buf_size));

        GHelpers::Frame::send(cfg, wbuf, tcp_fd, buf, buf_size);
    }
}

static
void P_relay_tcp_to_unix(
        const Config::Storage& cfg,
        int dbus_fd,
        int tcp_fd
)
{
    uint32_t len_be = 0;
    std::vector<uint8_t> payload;

    while(Utils::io::read_exact(tcp_fd, len_be))
    {
        const uint32_t len = be32toh(len_be);
        if(len > StaticConfig::MAX_FRAME)
        {
            break;
        }

        payload.reserve(len);
        if(!Utils::io::read_exact(tcp_fd, payload.data(), len))
        {
            break;
        }

        DEBUG_CALL(GHelpers::hexprint("TCP -> DBUS: ", payload.data(), len));

        Utils::io::write_exact(dbus_fd, payload.data(), len);
    }
}

void run(const Config::Storage& cfg)
{
    const int dbus_listen_fd = P_unix_listen(cfg.listen_socket);
    APPLOG_INFO("[Client] Listening on %s", cfg.listen_socket.c_str());

    while (true)
    {
        const int dbus_fd = accept(dbus_listen_fd, nullptr, nullptr);
        if (dbus_fd < 0)
        {
            continue;
        }

        APPLOG_DEBUG("Client: accepted");

        const pid_t proc_manager = fork();
        if(proc_manager != 0)
        {
            close(dbus_fd);
            int status;
            waitpid(proc_manager, &status, 0);
        }
        else
        {
            DEBUG_PRINT("Client: manager started");
            close(dbus_listen_fd);
            try
            {
                DEBUG_PRINT("Client: connection to %s", cfg.tcp_endpoint.c_str());
                const int tcp_fd = P_tcp_connect(cfg.tcp_endpoint);
                DEBUG_PRINT("Client: connected!");
                const pid_t p1 = fork();
                if (p1 == 0)
                {
                    DEBUG_PRINT("Client: process DBUS->TCP started");
                    prctl(PR_SET_PDEATHSIG, SIGKILL);
                    P_relay_unix_to_tcp(cfg, dbus_fd, tcp_fd);
                    DEBUG_PRINT("Client: process DBUS->TCP stopped");
                    exit(0);
                }

                const pid_t p2 = fork();
                if (p2 == 0)
                {
                    DEBUG_PRINT("Client: process TCP->DBUS started");
                    prctl(PR_SET_PDEATHSIG, SIGKILL);
                    P_relay_tcp_to_unix(cfg, dbus_fd, tcp_fd);
                    DEBUG_PRINT("Client: process TCP->DBUS stopped");
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

                DEBUG_PRINT("Client: manager stopped");
            }
            catch (const std::exception& e)
            {
                APPLOG_ERROR("[Client child error] %s", e.what());
            }
            exit(0);
        }
    }
}

} /* namespace Client */
} /* namespace App */
