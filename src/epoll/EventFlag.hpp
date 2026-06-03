/*
 * EventFlag.hpp
 *
 *  Created on: 31 мая 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_HELPERS_EPOLL_EVENTFLAG_HPP_
#define SRC_HELPERS_EPOLL_EVENTFLAG_HPP_

#include <sys/epoll.h>

namespace App
{
namespace Epoll
{
namespace EventFlag
{

/*
 * https://man7.org/linux/man-pages/man2/epoll_ctl.2.html
 *
 * Event flags are:
 * EPOLLET
 * EPOLLONESHOT (since Linux 2.6.2)
 * EPOLLWAKEUP (since Linux 3.5)
 * EPOLLEXCLUSIVE (since Linux 4.5)
 */

/** @brief Event flags */
enum Flags
{
    ET = EPOLLET,
    ONESHOT = EPOLLONESHOT,
    WAKEUP = EPOLLWAKEUP,
    EXCLUSIVE = EPOLLEXCLUSIVE,
};

} /* namespace EventFlag */
} /* namespace Epoll */
} /* namespace App */

#endif /* SRC_HELPERS_EPOLL_EVENTFLAG_HPP_ */
