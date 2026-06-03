/*
 * Handler.cpp
 *
 *  Created on: 30 мая 2026 г.
 *      Author: mastersan
 */

#include "epoll/Handler.hpp"
#include "epoll/HandlerCtrl.hpp"
#include "epoll/Ctrl.hpp"

#include <stdexcept>

namespace App
{
namespace Epoll
{

Handler::~Handler() noexcept
{
    if(m_fd >= 0 && m_ev.events != 0)
    {
        struct epoll_event tmp = m_ev;
        tmp.events = 0;
        (void)epoll_ctl(m_ctrl.m_epollfd, EPOLL_CTL_DEL, m_fd, &tmp);
    }
    m_fd = -1;
    for (auto& f : m_funcs)
    {
        f = {};
    }
}

class HandlerCtrl Handler::make_ctrl() noexcept
{
    return HandlerCtrl(*this, m_ctrl);
}

} /* namespace Epoll */
} /* namespace App */
