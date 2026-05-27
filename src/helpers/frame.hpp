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
#include "utils/io.hpp"

namespace App
{
namespace GHelpers
{
namespace Frame
{

static inline
void send(
        const Config::Storage& cfg,
        WriteBuffer& wbuf,
        int fd,
        const uint8_t * data,
        uint32_t size
)
{
    const uint32_t size_be = htobe32(size);
    DEBUG_PRINT("frame_send[raw]: len = %" PRIu32, size);
    wbuf.push(size_be);
    wbuf.push(data, size);
    Utils::io::write_exact(fd, wbuf.data(), wbuf.size());
    wbuf.clear();
}

static inline
void send(
        const Config::Storage& cfg,
        WriteBuffer& wbuf,
        int fd,
        const std::string& str
)
{
    const uint32_t size = str.size();
    const uint32_t size_be = htobe32(size);
    DEBUG_PRINT("frame_send[std::string]: len = %" PRIu32, size);
    wbuf.push(size_be);
    wbuf.push(str);
    Utils::io::write_exact(fd, wbuf.data(), wbuf.size());
    wbuf.clear();
}

template<typename Type>
void send(
        const Config::Storage& cfg,
        WriteBuffer& wbuf,
        int fd,
        const Type& data
)
{
    const uint32_t size = sizeof(Type);
    const uint32_t size_be = htobe32(size);
    DEBUG_PRINT("frame_send[Type]: len = %" PRIu32, size);
    wbuf.push(size_be);
    wbuf.push(data);
    Utils::io::write_exact(fd, wbuf.data(), wbuf.size());
    wbuf.clear();
}

} /* namespace Frame */
} /* namespace GHelpers */
} /* namespace App */

#endif /* SRC_HELPERS_FRAME_HPP_ */
