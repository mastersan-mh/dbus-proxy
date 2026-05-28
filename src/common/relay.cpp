
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

class Chan final
{
public:
    CLASS_NO_COPY(Chan);
    CLASS_NO_MOVE(Chan);

    Chan() = delete;

    Chan(
            Common::Types::ChanId chan_id,
            Epoll::Handler& dbus_handler
    )
    : m_chan_id(chan_id)
    , m_dbus_handler(dbus_handler)
    {}

    Common::Types::ChanId chan_id() const noexcept
    { return m_chan_id; }

    Epoll::Handler& dbus_handler() noexcept
    { return m_dbus_handler; }

private:
    const Common::Types::ChanId m_chan_id;
    Epoll::Handler& m_dbus_handler;
};

static
GHelpers::Socket::SendErr P_try_send_to_dbus(
        DbusFd fd,
        const Chan& chan,
        Frame::Unpacker& wbuf
)
{
    return GHelpers::Socket::try_send(
            fd,
            wbuf.output(chan.chan_id())
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
        DbusFd fd,
        Chan& chan,
        Frame::Unpacker& wbuf
)
{
    const GHelpers::Socket::SendErr send_res = P_try_send_to_dbus(fd, chan, wbuf);
    switch(send_res)
    {
        case GHelpers::Socket::SendErr::OK:
        {
            /* buffer is empty, remove from epoll */
            if(wbuf.empty(chan.chan_id()))
            {
                chan.dbus_handler()
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
        const std::map<DbusFd, Common::Types::ChanId>& channels_conf,
        int tcp_fd
)
{
    Epoll::Ctrl epoll(8);

    std::map<DbusFd, Chan> channels;

    std::set<Common::Types::ChanId> channels_id;
    for(const auto& [dbus_fd, chan_id] : channels_conf)
    {
        channels_id.emplace(chan_id);
    }

    GHelpers::WriteBuffer wbuf_dbus_to_tcp;
    Frame::Unpacker unpacker(channels_id);

    GHelpers::Socket::setnonblock(tcp_fd);
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
        Chan& chan = channels.at(fd);
        alive = P_event_send_to_dbus(fd, chan, unpacker);
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

            for(auto& [dbus_fd, chan] : channels)
            {
                if(!unpacker.empty(chan.chan_id()))
                {
                    const GHelpers::Socket::SendErr send_res =
                            P_try_send_to_dbus(dbus_fd, chan, unpacker);
                    switch(send_res)
                    {
                        case GHelpers::Socket::SendErr::OK: return;
                        case GHelpers::Socket::SendErr::AGAIN:
                        {
                            chan.dbus_handler()
                            .make_ctrl()
                            .ctl_add(Epoll::EventType::OUT, on_dbus_send)
                            .commit();
                            break;
                        }
                        case GHelpers::Socket::SendErr::ERROR: alive = false; return;
                    }
                }
            }
        }
    };

    auto on_dbus_recv = [&](int fd){
        static const size_t buf_capacity = 4096;
        uint8_t buf[buf_capacity];

        const Chan& chan = channels.at(fd);

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

            Frame::build(cfg, wbuf_dbus_to_tcp, chan.chan_id(), buf, buf_size);

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


    for(const auto& [dbus_fd, chan_id] : channels_conf)
    {
        GHelpers::Socket::setnonblock(dbus_fd);
        auto& dbus_handler = epoll.handler_create(dbus_fd);

        channels.try_emplace(
                dbus_fd,
                chan_id,
                dbus_handler
        );

        dbus_handler
        .make_ctrl()
        .set_flags(Epoll::EventFlag::ET)
        .ctl_add(Epoll::EventType::IN   , on_dbus_recv)
        .ctl_add(Epoll::EventType::ERR  , on_dbus_disconnect)
        .ctl_add(Epoll::EventType::HUP  , on_dbus_disconnect)
        .ctl_add(Epoll::EventType::RDHUP, on_dbus_disconnect)
        .commit();
    }

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
