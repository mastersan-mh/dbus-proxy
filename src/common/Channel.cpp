/*
 * Channel.cpp
 *
 */

#include "common/Channel.hpp"
#include "epoll/HandlerCtrl.hpp"

namespace App
{
namespace Common
{

Socket::SendErr Channel::try_send_to_dbus(
        Buffer::WriteBuffer& wbuf
) const noexcept
{
    return Socket::try_send(
            m_fd,
            wbuf
    );
}

bool Channel::event_send_to_dbus(
        Buffer::WriteBuffer& wbuf
)
{
    const Socket::SendErr send_res = try_send_to_dbus(wbuf);
    switch(send_res)
    {
        case Socket::SendErr::OK:
        {
            /* buffer is empty, remove from epoll */
            if(wbuf.empty())
            {
                m_dbus_handler
                .make_ctrl()
                .ctl_del(Epoll::EventType::OUT)
                .commit();
            }
            return true;
        }
        case Socket::SendErr::AGAIN: return true;
        case Socket::SendErr::ERROR: return false;
    }
    return false;
};

} /* namespace Common */
} /* namespace App */
