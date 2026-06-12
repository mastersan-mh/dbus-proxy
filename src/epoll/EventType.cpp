/*
 * utils.cpp
 *
 *  Created on: 13 июн. 2026 г.
 *      Author: mastersan
 */

#include "epoll/EventType.hpp"

namespace App
{
namespace Epoll
{

const char * event_type_to_string(EventType type)
{
    switch(type)
    {
        case EventType::IN   : return "EPOLLIN";
        case EventType::OUT  : return "EPOLLOUT";
        case EventType::RDHUP: return "EPOLLRDHUP";
        case EventType::PRI  : return "EPOLLPRI";
        case EventType::ERR  : return "EPOLLERR";
        case EventType::HUP  : return "EPOLLHUP";
    }
    return "<Unknown>";
}

} /* namespace Epoll */
} /* namespace App */
