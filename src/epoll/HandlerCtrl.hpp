/*
 * HandlerCtrl.hpp
 *
 *  Created on: 31 мая 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_HELPERS_EPOLL_HANDLERCTRL_HPP_
#define SRC_HELPERS_EPOLL_HANDLERCTRL_HPP_

#include "epoll/EventType.hpp"
#include "epoll/EventFlag.hpp"
#include "epoll/types.hpp"
#include "epoll/Handler.hpp"
#include "utils/class.hpp"

namespace App
{
namespace Epoll
{

class Ctrl;
class Handler;

class HandlerCtrl final
{
public:

    CLASS_NO_COPY(HandlerCtrl);
    CLASS_NO_MOVE(HandlerCtrl);

    HandlerCtrl() = delete;
    HandlerCtrl(Handler& handler, Ctrl& ctrl) noexcept;
    ~HandlerCtrl() noexcept = default;

    /** @brief Add event */
    HandlerCtrl& on_error(ErrorHandler func)
    {
        m_handler.m_error_handler = std::move(func);
        return *this;
    }

    /** @brief Add event */
    HandlerCtrl& ctl_add(EventType event, EventHandler func)
    {
        const uint32_t mask = static_cast<uint32_t>(event);
        m_target |= mask;
        if(func)
        {
            const size_t idx = P_event_type_to_index(event);
            m_handler.m_funcs[idx] = std::move(func);
        }
        return *this;
    }

    /** @brief Remove event */
    HandlerCtrl& ctl_del(EventType event)
    {
        const uint32_t mask = static_cast<uint32_t>(event);
        m_target &= ~mask;
        const size_t idx = P_event_type_to_index(event);
        m_handler.m_funcs[idx] = {};
        return *this;
    }

    /** @brief Set flags (EPOLLET и etc.) */
    constexpr
    HandlerCtrl& set_flags(EventFlag::Flags flags)
    { m_target |= flags; return *this; }

    /** @brief Remove flags */
    constexpr
    HandlerCtrl& clear_flags(EventFlag::Flags flags)
    { m_target &= ~flags; return *this; }

    /** @brief Commit changes */
    void commit();

private:
    Handler& m_handler;
    Ctrl&    m_ctrl;
    uint32_t m_target = 0;

    static constexpr
    size_t P_event_type_to_index(EventType et) noexcept
    {
        switch(et)
        {
            case EventType::IN   : return 0;
            case EventType::OUT  : return 1;
            case EventType::RDHUP: return 2;
            case EventType::PRI  : return 3;
            case EventType::ERR  : return 4;
            case EventType::HUP  : return 5;
        }
        return 0;
    }

};

} /* namespace Epoll */
} /* namespace App */

#endif /* SRC_HELPERS_EPOLL_HANDLERCTRL_HPP_ */
