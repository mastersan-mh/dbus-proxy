/*
 * frame.hpp
 *
 *  Created on: 27 мая 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_HELPERS_FRAME_HPP_
#define SRC_HELPERS_FRAME_HPP_

#include "helpers/debug.hpp"
#include "helpers/WriteBuffer.hpp"

namespace App
{
namespace GHelpers
{
namespace Frame
{

static inline
void build(
        const Config::Storage& cfg,
        WriteBuffer& wbuf,
        const uint8_t * data,
        uint32_t size
)
{
    const uint32_t size_be = htobe32(size);
    DEBUG_PRINT(cfg, "build[raw]: len = %" PRIu32, size);
    wbuf.push(size_be);
    wbuf.push(data, size);
}

} /* namespace Frame */
} /* namespace GHelpers */
} /* namespace App */

#endif /* SRC_HELPERS_FRAME_HPP_ */
