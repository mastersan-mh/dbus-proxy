/*
 * debug.hpp
 *
 *  Created on: 19 мая 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_HELPERS_DEBUG_HPP_
#define SRC_HELPERS_DEBUG_HPP_

#include "config_commandline/Storage.hpp"
#include "logger.hpp"

#include <functional>

namespace App
{
namespace GHelpers
{


static inline
void debug_call(
        const Config::Storage& cfg,
        std::function<void()> debug
)
{
    if(cfg.debug)
    {
        debug();
    }
}

#define DEBUG_CALL(func) \
        ::App::GHelpers::debug_call(cfg, [&](){ func; } )

#define DEBUG_PRINT(format, ...) do{ \
        if(cfg.debug){ APPLOG_DEBUG(format, ##__VA_ARGS__); } \
}while(0)

} /* namespace GHelpers */
} /* namespace App */

#endif /* SRC_HELPERS_DEBUG_HPP_ */
