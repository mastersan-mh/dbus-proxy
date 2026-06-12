/*
 * Unpacker.cpp
 *
 *  Created on: 2 июн. 2026 г.
 *      Author: mastersan
 */

#include "frame/Unpacker.hpp"

namespace App
{
namespace Frame
{

Unpacker::Unpacker(const std::set<Common::Types::ChanId>& channels)
{
    for(const auto chan_id : channels)
    {
        m_out_buf.try_emplace(chan_id);
    }
}

void Unpacker::push(const void* data, size_t size)
{
    if (size == 0) return;
    m_in_buf.push(data, size);
    P_process();
}

void Unpacker::P_process()
{
    Header header;
    static const size_t header_size = sizeof(Header);

    while(!m_in_buf.empty())
    {
        if(!m_header_parsed)
        {
            if (m_in_buf.size() < header_size)
            {
                return;
            }

            std::memcpy(&header, m_in_buf.data(), header_size);
            m_payload_remaining = be32toh(header.payload_size);

            m_in_buf.strip_begin(header_size);
            m_header_parsed = true;

            /* If len == 0, frame is empty */
            if (m_payload_remaining == 0)
            {
                m_header_parsed = false;
                continue;
            }
        }

        const size_t to_copy = std::min(m_in_buf.size(), m_payload_remaining);

        const auto ch_it = m_out_buf.find(header.channel);
        if(ch_it != m_out_buf.end())
        {
            /* store only for existing channels */
            ch_it->second.push(m_in_buf.data(), to_copy);
        }
        m_in_buf.strip_begin(to_copy);
        m_payload_remaining -= to_copy;

        if (m_payload_remaining == 0)
        {
            /* Current frame was totally sended */
            m_header_parsed = false;
        }
        else
        {
            break;
        }
    }
}

} /* namespace Frame */
} /* namespace App */
