/*
 * Channel.hpp
 *
 *  Created on: 13 июн. 2026 г.
 *      Author: mastersan
 */

#pragma once

#include "common/DbusFd.hpp"
#include "common/ChanId.hpp"
#include "epoll/Handler.hpp"
#include "socket/socket.hpp"
#include "frame/Unpacker.hpp"
#include "config_storage/Storage.hpp"
#include "utils/class.hpp"

namespace App
{
namespace Common
{

class Channel final
{
public:
    CLASS_NO_COPY(Channel);
    CLASS_NO_MOVE(Channel);

    Channel() = delete;

    explicit Channel(
            DbusFd fd,
            Types::ChanId chan_id,
            Epoll::Handler& dbus_handler
    )
    : m_dbus_handler(dbus_handler)
    , m_fd(fd)
    , m_chan_id(chan_id)
    {}

    Common::Types::ChanId chan_id() const noexcept
    { return m_chan_id; }

    Epoll::Handler& dbus_handler() noexcept
    { return m_dbus_handler; }

    void send_to_dbus(
            const Config::Storage& cfg,
            Buffer::WriteBuffer& wbuf
    );

private:
    Epoll::Handler& m_dbus_handler;
    const DbusFd m_fd;
    const Common::Types::ChanId m_chan_id;

    Socket::SendErr P_try_send_to_dbus(
            Buffer::WriteBuffer& wbuf
    ) const noexcept;

    void P_event_send_to_dbus(
            Buffer::WriteBuffer& wbuf
    );

};

} /* namespace Common */
} /* namespace App */
