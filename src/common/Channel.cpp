/*
 * Channel.cpp
 *
 */

#include "common/Channel.hpp"
#include "epoll/HandlerCtrl.hpp"
#include "helpers/debug.hpp"

namespace App
{
namespace Common
{

Socket::SendErr Channel::P_try_send_to_dbus(
        Buffer::WriteBuffer& wbuf
) const noexcept
{
    return Socket::try_send(
            m_fd,
            wbuf
    );
}

void Channel::P_event_send_to_dbus(
        Buffer::WriteBuffer& wbuf
)
{
    const Socket::SendErr send_res = P_try_send_to_dbus(wbuf);
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
            return;
        }
        case Socket::SendErr::AGAIN: return;
        case Socket::SendErr::ERROR:
        {
            throw std::runtime_error("Channel error");
        }
    }

    throw std::runtime_error("Channel error");
};

void Channel::send_to_dbus(
        const void* data,
        size_t size
)
{
    m_buffer.push(data, size);
    if(!m_buffer.empty())
    {
        const Socket::SendErr send_res =
                P_try_send_to_dbus(m_buffer);
        switch(send_res)
        {
            case Socket::SendErr::OK: return;
            case Socket::SendErr::AGAIN:
            {
                auto on_dbus_send = [this](int){
                    P_event_send_to_dbus(m_buffer);
                };
                m_dbus_handler
                .make_ctrl()
                .ctl_add(Epoll::EventType::OUT, on_dbus_send)
                .commit();
                break;
            }
            case Socket::SendErr::ERROR:
            {
                throw std::runtime_error("Channel::send_to_dbus() error");
            }
        }
    }

}

} /* namespace Common */
} /* namespace App */
