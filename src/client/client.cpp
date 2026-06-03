/*
 * client.cpp
 *
 *  Created on: 18 мая 2026 г.
 *      Author: mastersan
 */

#include "client/client.hpp"
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

        APPLOG_DEBUG("accepted");

        const pid_t proc_manager = fork();
        if(proc_manager != 0)
        {
            close(dbus_fd);
            int status;
            waitpid(proc_manager, &status, 0);
        }
        else
        {
            GHelpers::Thread::set_self_name("cl:instance");
            DEBUG_PRINT(cfg, "manager started");
            close(dbus_listen_fd);
            try
            {
                DEBUG_PRINT(cfg, "connection to %s", cfg.tcp_endpoint.c_str());
                const int tcp_fd = P_tcp_connect(cfg.tcp_endpoint);
                DEBUG_PRINT(cfg, "connected!");
                Common::relay(cfg, dbus_fd, tcp_fd);

                close(dbus_fd);
                shutdown(tcp_fd, SHUT_RDWR);
                close(tcp_fd);

                DEBUG_PRINT(cfg, "manager stopped");
            }
            catch (const std::exception& e)
            {
                APPLOG_ERROR("[Child error] %s", e.what());
            }
            exit(0);
        }
    }
}

} /* namespace Client */
} /* namespace App */
