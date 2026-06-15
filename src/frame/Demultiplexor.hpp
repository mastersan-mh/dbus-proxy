/*
 * Demultiplexor.hpp
 *
 *  Created on: 2 июн. 2026 г.
 *      Author: mastersan
 */

#pragma once

#include "buffer/WriteBuffer.hpp"
#include "common/ChanId.hpp"
#include "common/Channel.hpp"
#include "frame/Header.hpp"
#include "utils/class.hpp"

#include <cstdint>
#include <cstddef>
#include <cstring>

#include <stdexcept>
#include <memory>
#include <map>
#include <endian.h>

namespace App
{
namespace Frame
{

/**
 * @brief Demultiplexor
 */
class Demultiplexor
{
public:
    CLASS_NO_COPY(Demultiplexor);
    CLASS_NO_MOVE(Demultiplexor);

    Demultiplexor() = default;

    void register_channel(
            std::shared_ptr<Common::Channel>& channel
    )
    {
        m_channels.try_emplace(
                channel->chan_id(),
                channel
        );
    }

    /** @brief Append raw bytes */
    void push(const void* data, size_t size);

private:

    Buffer::WriteBuffer m_in_buf{};
    std::map<Common::Types::ChanId, std::shared_ptr<Common::Channel>> m_channels{};
    bool m_header_parsed = false;
    size_t m_payload_remaining = 0;

    void P_process();
};

} /* namespace GHelpers */
} /* namespace App */

