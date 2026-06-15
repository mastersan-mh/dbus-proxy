/*
 * WriteBuffer.hpp
 *
 *  Created on: 27 may 2026 г.
 *      Author: mastersan
 */

#pragma once

#include "utils/class.hpp"

#include <algorithm>
#include <cstring>
#include <type_traits>

#include <string>
#include <vector>
#include <stddef.h>
#include <stdint.h>

namespace App
{
namespace Buffer
{

class WriteBuffer final
{
public:

    CLASS_NO_COPY(WriteBuffer);
    CLASS_NO_MOVE(WriteBuffer);

    WriteBuffer() = default;

    explicit WriteBuffer(size_t reserve)
    { m_data.reserve(reserve); }

    ~WriteBuffer() = default;

    bool empty() const noexcept
    { return m_read_ofs >= m_data.size(); }

    size_t size() const noexcept
    { return (m_data.size() - m_read_ofs); }

    /**
     * @brief Beginning of the data
     */
    const void* data() const noexcept
    { return static_cast<const void*>(m_data.data() + m_read_ofs); }

    /**
     * @brief Pointer to the beginning of write area
     */
    void* write_ptr() noexcept
    { return static_cast<void*>(m_data.data() + m_data.size()); }

    /**
     * @brief How many we can write without reallocation
     */
    size_t writable_capacity() const noexcept
    { return m_data.capacity() - m_data.size(); }

    void clear() noexcept
    {
        m_data.clear();
        m_read_ofs = 0;
    }

    /**
     * @brief Remove n bytes from the beginning.
     */
    void strip_begin(size_t n);

    /**
     * @brief Force compacting
     */
    void compact() noexcept;

    void push(const void* src_data, size_t data_size);

    void push(const std::string& str)
    { push(str.data(), str.size()); }

    template<typename Type>
    void push(const Type& val)
    {
        static_assert(std::is_trivially_copyable_v<Type>, "Type must be trivially copyable");
        push(static_cast<const void*>(&val), sizeof(Type));
    }

private:
    std::vector<uint8_t> m_data{};
    size_t m_read_ofs = 0; /**< Offset of logical buffer inside vector */
};

} /* namespace Buffer */
} /* namespace App */

