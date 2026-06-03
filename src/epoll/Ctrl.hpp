/*
 * Ctrl.hpp
 *
 *  Created on: 30 мая 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_HELPERS_EPOLL_CTRL_HPP_
#define SRC_HELPERS_EPOLL_CTRL_HPP_

#include "epoll/Handler.hpp"
#include "epoll/HandlerCtrl.hpp"
#include "utils/class.hpp"

#include <map>
#include <sys/epoll.h>

namespace App
{
namespace Epoll
{

class HandlerCtrl;

class Ctrl final
{
    friend Handler;
    friend HandlerCtrl;
public:

    CLASS_NO_COPY(Ctrl);
    CLASS_NO_MOVE(Ctrl);

    Ctrl() = delete;
    Ctrl(size_t max_events = 8);
    ~Ctrl() = default;

    Handler& handler_create(int fd);

    void wait();

private:

    int m_epollfd{};

    uint64_t m_hid = 0;
    const size_t m_conf_max_events;

    std::map<uint64_t, Handler> m_handlers{};

    std::vector<struct epoll_event> m_events{};

};

} /* namespace Epoll */
} /* namespace App */

#endif /* SRC_HELPERS_EPOLL_CTRL_HPP_ */
