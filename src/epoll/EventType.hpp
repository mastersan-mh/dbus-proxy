/*
 * EventType.hpp
 *
 *  Created on: 31 мая 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_HELPERS_EPOLL_EVENTTYPE_HPP_
#define SRC_HELPERS_EPOLL_EVENTTYPE_HPP_

#include <stdint.h>
#include <sys/epoll.h>

namespace App
{
namespace Epoll
{

/*
 * https://man7.org/linux/man-pages/man2/epoll_ctl.2.html
 *
 * Event types are:
 * EPOLLIN
 * EPOLLOUT
 * EPOLLRDHUP
 * EPOLLPRI
 * EPOLLERR
 * EPOLLHUP
 */

/** @brief Event types */
enum class EventType : uint32_t
{
    IN    = EPOLLIN,
    OUT   = EPOLLOUT,
    RDHUP = EPOLLRDHUP,
    PRI   = EPOLLPRI,
    ERR   = EPOLLERR,
    HUP   = EPOLLHUP,
};

const char * event_type_to_string(EventType type);

} /* namespace Epoll */
} /* namespace App */

#endif /* SRC_HELPERS_EPOLL_EVENTTYPE_HPP_ */
