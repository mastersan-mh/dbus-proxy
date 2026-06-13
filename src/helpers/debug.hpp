/*
 * debug.hpp
 *
 *  Created on: 19 мая 2026 г.
 *      Author: mastersan
 */

#pragma once

#include "config_storage/Storage.hpp"
#include "logger.hpp"

#include <functional>

namespace App
{
namespace GHelpers
{


static inline
void debug_call(
        const Config::Storage& cfg,
        unsigned level,
        std::function<void()> debug
)
{
    if(cfg.debug >= level)
    {
        debug();
    }
}

#define DEBUG_CALL(x_cfg, x_level, x_func) \
        ::App::GHelpers::debug_call(x_cfg, x_level, [&](){ x_func; } )

#define DEBUG_PRINT(x_cfg, x_level, x_format, ...) do{ \
        if(x_cfg.debug >= x_level){ APPLOG_DEBUG(x_format, ##__VA_ARGS__); } \
}while(0)

} /* namespace GHelpers */
} /* namespace App */
