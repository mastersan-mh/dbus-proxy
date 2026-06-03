/*
 * Handler.hpp
 *
 *  Created on: 30 мая 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_HELPERS_EPOLL_HANDLER_HPP_
#define SRC_HELPERS_EPOLL_HANDLER_HPP_

#include "utils/class.hpp"

#include <functional>
#include <sys/epoll.h>

namespace App
{
namespace Epoll
{

class Ctrl;
class HandlerCtrl;

class Handler final
{
    friend Ctrl;
    friend HandlerCtrl;
public:

    CLASS_NO_COPY(Handler);
    CLASS_NO_MOVE(Handler);

    using Func = std::function<void(int fd)>;

    Handler() = delete;
    Handler(
            Ctrl& ctrl,
            uint64_t id,
            int fd
    ) noexcept
    : m_ctrl(ctrl)
    , m_fd(fd)
    {
        m_ev.data.u64 = id;
    }


    ~Handler();

    uint64_t id() const noexcept
    { return m_ev.data.u64; }

    uint64_t fd() const noexcept
    { return m_fd; }

    HandlerCtrl make_ctrl() noexcept;

private:

    Ctrl& m_ctrl;
    int m_fd;
    struct epoll_event m_ev{};
    std::array<Func, 6> m_funcs{};
};

} /* namespace Epoll */
} /* namespace App */

#endif /* SRC_HELPERS_EPOLL_HANDLER_HPP_ */
