
#include "common/relay.hpp"

#include "config_static/Storage.hpp"
#include "epoll/Ctrl.hpp"
#include "frame/builder.hpp"
#include "frame/Unpacker.hpp"
#include "helpers/hexprinter.hpp"
#include "helpers/socket.hpp"
#include "helpers/debug.hpp"

#include <stdexcept>
#include <string.h>
#include <endian.h>

namespace App
{
namespace Common
{

static
GHelpers::Socket::SendErr P_try_send_to_dbus(
        int fd,
        Frame::Unpacker& wbuf
)
{
    return GHelpers::Socket::try_send(
            fd,
            wbuf.output()
    );
}

static
GHelpers::Socket::SendErr P_try_send_to_tcp(
        int fd,
        GHelpers::WriteBuffer& wbuf
)
{
    return GHelpers::Socket::try_send(
            fd,
            wbuf
    );
}

static
bool P_event_send_to_tcp(
        int fd,
        GHelpers::WriteBuffer& wbuf,
        Epoll::Handler& handler
)
{
    const GHelpers::Socket::SendErr send_res = P_try_send_to_tcp(fd, wbuf);
    switch(send_res)
    {
        case GHelpers::Socket::SendErr::OK:
        {
            /* buffer is empty, remove from epoll */
            if(wbuf.empty())
            {
                handler
                .make_ctrl()
                .ctl_del(Epoll::EventType::OUT)
                .commit();
            }
            return true;
        }
        case GHelpers::Socket::SendErr::AGAIN: return true;
        case GHelpers::Socket::SendErr::ERROR: return false;
    }
    return false;
};

static
bool P_event_send_to_dbus(
        int fd,
        Frame::Unpacker& wbuf,
        Epoll::Handler& handler
)
{
    const GHelpers::Socket::SendErr send_res = P_try_send_to_dbus(fd, wbuf);
    switch(send_res)
    {
        case GHelpers::Socket::SendErr::OK:
        {
            /* buffer is empty, remove from epoll */
            if(wbuf.empty())
            {
                handler
                .make_ctrl()
                .ctl_del(Epoll::EventType::OUT)
                .commit();
            }
            return true;
        }
        case GHelpers::Socket::SendErr::AGAIN: return true;
        case GHelpers::Socket::SendErr::ERROR: return false;
    }
    return false;
};

void relay(
        const Config::Storage& cfg,
        int dbus_fd,
        int tcp_fd
)
{
    Epoll::Ctrl epoll(8);

    GHelpers::Socket::setnonblock(dbus_fd);
    GHelpers::Socket::setnonblock(tcp_fd);

    GHelpers::WriteBuffer wbuf_dbus_to_tcp;
    Frame::Unpacker unpacker;

    auto& dbus_handler = epoll.handler_create(dbus_fd);
    auto& tcp_handler = epoll.handler_create(tcp_fd);

    bool alive = true;
    auto on_dbus_disconnect = [&](int){
        DEBUG_PRINT(cfg, "on_dbus_disconnect");
        alive = false;
    };

    auto on_tcp_disconnect = [&](int){
        DEBUG_PRINT(cfg, "on_tcp_disconnect");
        alive = false;
    };

    auto on_tcp_send = [&](int fd){
        DEBUG_PRINT(cfg, "on_tcp_send");
        alive = P_event_send_to_tcp(fd, wbuf_dbus_to_tcp, tcp_handler);
    };

    auto on_dbus_send = [&](int fd){
        DEBUG_PRINT(cfg, "on_dbus_send");
        alive = P_event_send_to_dbus(fd, unpacker, dbus_handler);
    };

    auto on_tcp_recv = [&](int fd){
        static const size_t buf_capacity = 4096;
        uint8_t buf[buf_capacity];

        while(alive)
        {
            size_t buf_size;
            const GHelpers::Socket::RecvErr res_recv =
                    GHelpers::Socket::try_recv(fd, buf, sizeof(buf), buf_size);
            switch(res_recv)
            {
                case GHelpers::Socket::RecvErr::OK: break;
                case GHelpers::Socket::RecvErr::AGAIN: return;
                case GHelpers::Socket::RecvErr::ERROR: alive = false; return;
            }

            unpacker.push(buf, buf_size);

            DEBUG_CALL(cfg, GHelpers::hexprint("TCP -> DBUS: ", buf, buf_size));

            if(!unpacker.empty())
            {
                const GHelpers::Socket::SendErr send_res =
                        P_try_send_to_dbus(dbus_fd, unpacker);
                switch(send_res)
                {
                    case GHelpers::Socket::SendErr::OK: return;
                    case GHelpers::Socket::SendErr::AGAIN:
                    {
                        dbus_handler
                        .make_ctrl()
                        .ctl_add(Epoll::EventType::OUT, on_dbus_send)
                        .commit();
                        break;
                    }
                    case GHelpers::Socket::SendErr::ERROR: alive = false; return;
                }
            }
        }
    };

    auto on_dbus_recv = [&](int fd){
        static const size_t buf_capacity = 4096;
        uint8_t buf[buf_capacity];

        while(alive)
        {
            size_t buf_size;
            const GHelpers::Socket::RecvErr res_recv =
                    GHelpers::Socket::try_recv(fd, buf, sizeof(buf), buf_size);
            switch(res_recv)
            {
                case GHelpers::Socket::RecvErr::OK: break;
                case GHelpers::Socket::RecvErr::AGAIN: return;
                case GHelpers::Socket::RecvErr::ERROR: alive = false; return;
            }

            DEBUG_CALL(cfg, GHelpers::hexprint("DBUS -> TCP: ", buf, buf_size));
            Frame::build(cfg, wbuf_dbus_to_tcp, buf, buf_size);

            const GHelpers::Socket::SendErr send_res =
                    P_try_send_to_tcp(tcp_fd, wbuf_dbus_to_tcp);
            switch(send_res)
            {
                case GHelpers::Socket::SendErr::OK: return;
                case GHelpers::Socket::SendErr::AGAIN:
                {
                    tcp_handler
                    .make_ctrl()
                    .ctl_add(Epoll::EventType::OUT, on_tcp_send)
                    .commit();
                    break;
                }
                case GHelpers::Socket::SendErr::ERROR: alive = false; return;
            }
        }
    };

    dbus_handler
    .make_ctrl()
    .set_flags(Epoll::EventFlag::ET)
    .ctl_add(Epoll::EventType::IN   , on_dbus_recv)
    .ctl_add(Epoll::EventType::ERR  , on_dbus_disconnect)
    .ctl_add(Epoll::EventType::HUP  , on_dbus_disconnect)
    .ctl_add(Epoll::EventType::RDHUP, on_dbus_disconnect)
    .commit();

    tcp_handler
    .make_ctrl()
    .set_flags(Epoll::EventFlag::ET)
    .ctl_add(Epoll::EventType::IN   , on_tcp_recv)
    .ctl_add(Epoll::EventType::ERR  , on_tcp_disconnect)
    .ctl_add(Epoll::EventType::HUP  , on_tcp_disconnect)
    .ctl_add(Epoll::EventType::RDHUP, on_tcp_disconnect)
    .commit();

    while(alive)
    {
        epoll.wait();
    }
}

} /* namespace Common */
} /* namespace App */
