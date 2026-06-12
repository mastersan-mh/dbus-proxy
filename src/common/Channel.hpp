/*
 * Channel.hpp
 *
 *  Created on: 13 июн. 2026 г.
 *      Author: mastersan
 */

#pragma once

#include "common/ChanId.hpp"
#include "epoll/Handler.hpp"
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

    Channel(
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

} /* namespace Common */
} /* namespace App */
