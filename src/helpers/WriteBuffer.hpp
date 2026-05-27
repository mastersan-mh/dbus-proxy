/*
 * WriteBuffer.hpp
 *
 *  Created on: 27 мая 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_HELPERS_WRITEBUFFER_HPP_
#define SRC_HELPERS_WRITEBUFFER_HPP_

#include <string>
#include <vector>
#include <stddef.h>
#include <stdint.h>

namespace App
{
namespace GHelpers
{

class WriteBuffer final
{
public:
    WriteBuffer() = default;
    WriteBuffer(size_t reserve)
    {
        m_data.reserve(reserve);
    }

    ~WriteBuffer() = default;

    const void * data() const noexcept
    { return reinterpret_cast<const void*>(m_data.data()); }

    size_t size() const noexcept
    { return m_data.size(); }

    void clear()
    { m_data.clear(); }

    void push(const void * data, size_t size)
    {
        const uint8_t * ptr = reinterpret_cast<const uint8_t*>(data);
        m_data.insert(m_data.end(), ptr, ptr + size);
    }

    void push(const std::string& str)
    {
        push(str.data(), str.size());
    }

    template<typename Type>
    void push(const Type& data)
    {
        static_assert(
                std::is_trivially_copyable_v<Type>,
                "write_exact requires trivially copyable type"
        );

        return push(reinterpret_cast<const uint8_t*>(&data), sizeof(Type));
    }

private:
    std::vector<uint8_t> m_data{};
};

} /* namespace GHelpers */
} /* namespace App */

#endif /* SRC_HELPERS_WRITEBUFFER_HPP_ */
