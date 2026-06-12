/*
 * Unpacker.hpp
 *
 *  Created on: 2 июн. 2026 г.
 *      Author: mastersan
 */

#pragma once

#include "buffer/WriteBuffer.hpp"
#include "common/ChanId.hpp"
#include "frame/Header.hpp"
#include "utils/class.hpp"

#include <cstdint>
#include <cstddef>
#include <cstring>

#include <stdexcept>
#include <set>
#include <map>
#include <endian.h>

namespace App
{
namespace Frame
{

class Unpacker
{
public:
    CLASS_NO_COPY(Unpacker);
    CLASS_NO_MOVE(Unpacker);
    Unpacker() = delete;

    Unpacker(const std::set<Common::Types::ChanId>& channels);

    /** @brief Append raw bytes */
    void push(const void* data, size_t size);

    /** @brief Get output buffer for channel with raw DBUS stream */
    Buffer::WriteBuffer& buffer(Common::Types::ChanId channel_id) noexcept
    { return P_buffer(channel_id); }

    const Buffer::WriteBuffer& buffer(Common::Types::ChanId channel_id) const noexcept
    { return P_buffer(channel_id); }

private:

    Buffer::WriteBuffer m_in_buf{};
    std::map<Common::Types::ChanId, Buffer::WriteBuffer> m_out_buf{}; /**< Output buffers for channels */
    bool m_header_parsed = false;
    size_t m_payload_remaining = 0;

    void P_process();

    Buffer::WriteBuffer& P_buffer(Common::Types::ChanId channel_id)
    {
        auto it = m_out_buf.find(channel_id);
        if(it == m_out_buf.end())
        {
            throw std::runtime_error("Requested invalid channel");
        }
        return it->second;
    }

    const Buffer::WriteBuffer& P_buffer(Common::Types::ChanId channel_id) const
    {
        auto it = m_out_buf.find(channel_id);
        if(it == m_out_buf.end())
        {
            throw std::runtime_error("Requested invalid channel");
        }
        return it->second;
    }

};

} /* namespace GHelpers */
} /* namespace App */

