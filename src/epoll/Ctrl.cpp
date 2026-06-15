/*
 * Ctrl.cpp
 *
 *  Created on: 30 мая 2026 г.
 *      Author: mastersan
 */

#include "epoll/Ctrl.hpp"
#include "epoll/Handler.hpp"

#include <stdexcept>

namespace App
{
namespace Epoll
{

static
EventType P_index_to_event_type_to_index(size_t index) noexcept
{
    switch(index)
    {
        case 0: return EventType::IN   ;
        case 1: return EventType::OUT  ;
        case 2: return EventType::RDHUP;
        case 3: return EventType::PRI  ;
        case 4: return EventType::ERR  ;
        case 5: return EventType::HUP  ;
    }
    return EventType::IN;
}

Ctrl::Ctrl(size_t max_events)
: m_conf_max_events(max_events)
{

    m_epollfd = epoll_create1(0);
    if(m_epollfd == -1)
    {
        throw std::runtime_error("epoll_create1()");
    }

    m_events.resize(m_conf_max_events);
}

Handler& Ctrl::handler_create(int fd)
{
    auto [it, success] = m_handlers.try_emplace(
            m_hid,
            *this,
            m_hid,
            fd
    );
    m_hid++;
    return it->second;
}

void Ctrl::wait()
{
    const int nfds = epoll_wait(m_epollfd, m_events.data(), m_conf_max_events, -1);
    if(nfds == -1)
    {
        if(errno == EINTR)
        {
            return;
        }

        throw std::runtime_error("epoll_wait()");
    }

    int ifd;
    for(ifd = 0 ; ifd < nfds; ++ifd)
    {
        struct epoll_event * event = &m_events[ifd];
        const size_t index = event->data.u64;

        const auto it = m_handlers.find(index);
        if(it != m_handlers.end())
        {
            Handler& handler = it->second;
            const uint32_t triggered = event->events;

            for(size_t ihandler = 0; ihandler < Handler::FUNCS_NUM; ++ihandler)
            {
                auto& func = handler.m_funcs[ihandler];
                if(
                        func &&
                        (triggered & (UINT32_C(1) << ihandler))
                )
                {
                    try
                    {
                        func(handler.m_fd);
                    }
                    catch(...)
                    {
                        if(handler.m_error_handler)
                        {
                            try
                            {
                                std::exception_ptr eptr = std::current_exception();
                                handler.m_error_handler(
                                        handler.m_fd,
                                        P_index_to_event_type_to_index(ihandler),
                                        eptr
                                );
                            }
                            catch(...)
                            {
                                /* Ignore all errors */
                            }
                        }
                    }
                }
            }
        }
    }
}

} /* namespace Epoll */
} /* namespace App */
