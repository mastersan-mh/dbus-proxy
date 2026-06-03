/*
 * HandlerCtrl.cpp
 *
 *  Created on: 31 мая 2026 г.
 *      Author: mastersan
 */

#include "epoll/HandlerCtrl.hpp"
#include "epoll/Handler.hpp"
#include "epoll/Ctrl.hpp"

#include <stdexcept>
#include <string>
#include <string.h>
#include <sys/epoll.h>
#include <errno.h>

namespace App
{
namespace Epoll
{

HandlerCtrl::HandlerCtrl(
        Handler& handler,
        Ctrl& ctrl
) noexcept
: m_handler(handler)
, m_ctrl(ctrl)
, m_target(handler.m_ev.events)
{}

void HandlerCtrl::commit()
{
    const uint32_t current = m_handler.m_ev.events;
    const uint32_t desired = m_target;

    if (current == desired)
    {
        /* Nothing changed */
        return;
    }

    int op;
    if (current == 0)      op = EPOLL_CTL_ADD;
    else if (desired == 0) op = EPOLL_CTL_DEL;
    else                   op = EPOLL_CTL_MOD;

    struct epoll_event tmp = m_handler.m_ev;
    tmp.events = desired;

    if(epoll_ctl(m_ctrl.m_epollfd, op, m_handler.m_fd, &tmp) == -1)
    {
        throw std::runtime_error("epoll_ctl commit failed: " + std::string(strerror(errno)));
    }

    /* sync */
    m_handler.m_ev.events = desired;
}

} /* namespace Epoll */
} /* namespace App */
