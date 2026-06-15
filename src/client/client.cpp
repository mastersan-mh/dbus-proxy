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
#include "socket/socket.hpp"
#include "helpers/ParseEndpoint.hpp"
#include "helpers/thread.hpp"
#include "helpers/debug.hpp"

#include <sys/wait.h>
#include <unistd.h>
#include <stdexcept>
#include <filesystem>
#include <set>

namespace App
{
namespace Client
{

static
int P_tcp_connect(const std::string& endpoint, uint16_t port)
{
    const struct sockaddr_in addr = GHelpers::build_sockaddr_in(endpoint, port);

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
    const struct sockaddr_un addr = GHelpers::build_sockaddr_un(path);

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

static
void P_on_accept(
        const Config::Storage& cfg,
        Common::Types::ChanId chan_id,
        int dbus_listen_fd
)
{
    const std::filesystem::path& bus_path = cfg.bus_mapping.at(chan_id);

    APPLOG_INFO("[Client] Listening on %s", bus_path.c_str());

    while(true)
    {
        const int dbus_fd = accept(dbus_listen_fd, nullptr, nullptr);
        if(dbus_fd < 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            APPLOG_WARN("accept() failed: %s", strerror(errno));
            break;
        }


        const pid_t proc_manager = fork();
        if(proc_manager != 0)
        {
            close(dbus_fd);
        }
        else
        {
            close(dbus_listen_fd);

            char thread_name[16];
            snprintf(thread_name, sizeof(thread_name), "cl:bus:%" PRIu8, chan_id);
            GHelpers::Thread::set_self_name(thread_name);

            try
            {
                const std::map<Common::DbusFd, Common::Types::ChanId> channels = {{dbus_fd, chan_id}};

                DEBUG_PRINT(cfg,"Accepted on %" PRIu8 ":%s",
                        chan_id,
                        bus_path.c_str()
                );

                const int tcp_fd = P_tcp_connect(cfg.addr, cfg.port);
                DEBUG_PRINT(cfg, "Connected to %s:%" PRIu16, cfg.addr.c_str(), cfg.port);

                Common::relay(cfg, channels, tcp_fd);

                shutdown(tcp_fd, SHUT_RDWR);
                close(tcp_fd);
            }
            catch (const std::exception& e)
            {
                APPLOG_ERROR("%s", e.what());
            }

            close(dbus_fd);

            DEBUG_PRINT(cfg, "Channel stopped");
            _exit(0);
        }
    }
}

void run(const Config::Storage& cfg)
{
    Epoll::Ctrl ctrl(64);

    signal(SIGCHLD, SIG_IGN);

    for(auto& [id, sock_path] : cfg.bus_mapping)
    {

        const int dbus_listen_fd = P_unix_listen(sock_path);
        Socket::setnonblock(dbus_listen_fd);

        auto on_accept = [&cfg, id](int fd){
            P_on_accept(cfg, id, fd);
        };

        Epoll::Handler& handler = ctrl.handler_create(dbus_listen_fd);

        handler
        .make_ctrl()
        .set_flags(Epoll::EventFlag::ET)
        .ctl_add(Epoll::EventType::IN, on_accept)
        .commit();

    }

    while(true)
    {
        ctrl.wait();
    }
}

} /* namespace Client */
} /* namespace App */
