/*
 * Unpacker.hpp
 *
 *  Created on: 2 июн. 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_CLIENT_UNPACKER_HPP_
#define SRC_CLIENT_UNPACKER_HPP_

#include "helpers/WriteBuffer.hpp"
#include "utils/class.hpp"

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <endian.h>

namespace App
{
namespace GHelpers
{

class Unpacker
{
public:
    CLASS_NO_COPY(Unpacker);
    CLASS_NO_MOVE(Unpacker);
    Unpacker() = default;

    /** @brief Append raw bytes */
    void push(const void* data, size_t size);

    /** @brief Is it empty? */
    bool empty() const noexcept
    { return m_out_buf.empty(); }

    /** @brief Получить ссылку на выходной буфер (содержит пейлоады без 4-байтовых заголовков) */
    GHelpers::WriteBuffer& output() noexcept
    { return m_out_buf; }

    const GHelpers::WriteBuffer& output() const noexcept
    { return m_out_buf; }

    /** @brief Full state reset */
    void reset() noexcept
    {
        m_in_buf.clear();
        m_out_buf.clear();
        m_header_parsed = false;
        m_payload_remaining = 0;
    }

private:
    void process();

    GHelpers::WriteBuffer m_in_buf{};
    GHelpers::WriteBuffer m_out_buf{};
    bool m_header_parsed = false;
    size_t m_payload_remaining = 0;
};

} /* namespace GHelpers */
} /* namespace App */

#endif /* SRC_CLIENT_UNPACKER_HPP_ */
