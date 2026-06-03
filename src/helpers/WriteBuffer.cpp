/*
 * WriteBuffer.hpp
 *
 *  Created on: 27 мая 2026 г.
 *      Author: mastersan
 */

#include "helpers/WriteBuffer.hpp"

namespace App
{
namespace GHelpers
{

void WriteBuffer::strip_begin(size_t n)
{
    if(n >= size())
    {
        clear(); // Удаляем всё или больше, чем есть
        return;
    }
    m_read_ofs += n;

    // Эвристика компакции:
    // Если "мёртвое" пространство в начале (m_read_ofs) больше или равно
    // полезным данным, то сдвигаем данные в начало.
    // Это освобождает место в конце вектора для будущих push() без realloc.
    if(m_read_ofs >= size())
    {
        m_read_ofs = size();
        compact();
    }
}

void WriteBuffer::compact()
{
    if (m_read_ofs == 0)
    {
        return;
    }
    // std::vector::erase реализует именно memmove, что нам и нужно
    m_data.erase(m_data.begin(), m_data.begin() + m_read_ofs);
    m_read_ofs = 0;
}

void WriteBuffer::push(const void* src_data, size_t data_size)
{
    if(data_size == 0)
    {
        return;
    }

    const size_t tail = m_data.capacity() - m_data.size();
    if(tail < data_size)
    {
        const size_t head = m_read_ofs;
        const size_t sz = size();
        if(head > sz)
        {
            compact();
        }
    }

    const uint8_t* ptr = static_cast<const uint8_t*>(src_data);
    m_data.insert(m_data.end(), ptr, ptr + data_size);
}

} /* namespace GHelpers */
} /* namespace App */

