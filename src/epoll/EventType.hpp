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

/**
 * @brief Event types
 * @note
 * See
 * https://man7.org/linux/man-pages/man2/epoll_ctl.2.html
 * for full description.
 */
enum class EventType : uint32_t
{
    IN    = EPOLLIN   , /**< Available to read. */
    OUT   = EPOLLOUT  , /**< Available to write. */
    RDHUP = EPOLLRDHUP, /**< Stream socket peer closed connection, or shut down writing half of connection. */
    PRI   = EPOLLPRI  , /**< There is some exceptional condition on the file descriptor. */
    ERR   = EPOLLERR  , /**< Error condition happened on the associated file descriptor. */
    HUP   = EPOLLHUP  , /**< Hang up happened on the associated file descriptor. */
};

const char * event_type_to_string(EventType type);

} /* namespace Epoll */
} /* namespace App */

#endif /* SRC_HELPERS_EPOLL_EVENTTYPE_HPP_ */
