/*
 * frame.hpp
 *
 *  Created on: 27 мая 2026 г.
 *      Author: mastersan
 */

#pragma once

#include "helpers/debug.hpp"
#include "buffer/WriteBuffer.hpp"
#include "frame/Header.hpp"
#include "logger.hpp"

namespace App
{
namespace Frame
{

static inline
void build(
        Buffer::WriteBuffer& wbuf,
        Common::Types::ChanId chan_id,
        const void * data,
        uint32_t size
)
{
    const Frame::Header header =
    {
            .payload_size = htobe32(size),
            .channel = chan_id,
            .gap0 = 0,
            .gap1 = 0,
            .gap2 = 0,
    };

    wbuf.push(&header, sizeof(header));
    wbuf.push(data, size);
}

} /* namespace Frame */
} /* namespace App */
