/*
 * server.cpp
 *
 *  Created on: 18 мая 2026 г.
 *      Author: mastersan
 */

#include "server/server.hpp"
#include "common/relay.hpp"
#include "logger.hpp"
#include "config_static/Storage.hpp"
#include "epoll/Ctrl.hpp"
#include "helpers/ParseEndpoint.hpp"
#include "helpers/thread.hpp"
#include "helpers/debug.hpp"

#include <stdexcept>
#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>

namespace App
{
namespace Server
{

static
int P_tcp_listen(const std::string& endpoint, uint16_t port)
{
    const struct sockaddr_in addr = GHelpers::build_sockaddr_in(endpoint, port);

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
int P_unix_connect(const std::filesystem::path& path)
{
    const int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(fd < 0)
    {
        throw std::runtime_error("socket() failed");
    }

    const struct sockaddr_un addr = GHelpers::build_sockaddr_un(path);

    if(connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        throw std::runtime_error("connect() failed");
    }

    return fd;
}

void run(const Config::Storage& cfg)
{
    const int listen_fd = P_tcp_listen(cfg.addr, cfg.port);
    APPLOG_INFO("[Server] Listening on %s:%" PRIu16, cfg.addr.c_str(), cfg.port);

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
            GHelpers::Thread::set_self_name("sv:instance");
            try
            {
                const int dbus_fd = P_unix_connect(cfg.dbus_socket);
                Common::relay(cfg, dbus_fd, tcp_fd);
                close(dbus_fd);
            }
            catch (const std::exception& e)
            {
                APPLOG_ERROR("[Child error] %s", e.what());
            }
            exit(0);
        }
    }
}

} /* namespace Server */
} /* namespace App */
