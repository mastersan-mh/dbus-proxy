/*
 * types.hpp
 *
 *  Created on: 13 июн. 2026 г.
 *      Author: mastersan
 */

#pragma once

#include <functional>
#include <exception>

namespace App
{
namespace Epoll
{

using EventHandler = std::function<void(int fd)>;
using ErrorHandler = std::function<void(
        int fd,
        EventType event_type,
        std::exception_ptr eptr
)>;

} /* namespace Epoll */
} /* namespace App */
