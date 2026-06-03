/*
 * Unpacker.cpp
 *
 *  Created on: 2 июн. 2026 г.
 *      Author: mastersan
 */

#include "helpers/Unpacker.hpp"

namespace App
{
namespace GHelpers
{

void Unpacker::push(const void* data, size_t size)
{
    if (size == 0) return;
    m_in_buf.push(data, size);
    process();
}

void Unpacker::process()
{
    while(!m_in_buf.empty())
    {
        if(!m_header_parsed)
        {
            // --- Фаза 1: Парсинг заголовка (<4: len>) ---
            if (m_in_buf.size() < 4)
            {
                return; // Ждём, пока накопится хотя бы 4 байта на заголовок
            }

            uint32_t len_be = 0;
            std::memcpy(&len_be, m_in_buf.data(), 4);
            m_payload_remaining = be32toh(len_be);

            m_in_buf.strip_begin(4); // Заголовок съеден, дальше идёт пейлоад
            m_header_parsed = true;

            // Если len == 0, фрейм пустой — сразу переходим к следующему заголовку
            if (m_payload_remaining == 0)
            {
                m_header_parsed = false;
                continue;
            }
        }

        // --- Фаза 2: Потоковая пересылка пейлоада ---
        // Копируем ВСЁ, что есть в m_in_buf, но не больше, чем осталось от текущего фрейма
        const size_t to_copy = std::min(m_in_buf.size(), m_payload_remaining);

        m_out_buf.push(m_in_buf.data(), to_copy); // Пересылаем в выходной буфер
        m_in_buf.strip_begin(to_copy);            // Удаляем из входного
        m_payload_remaining -= to_copy;           // Уменьшаем счётчик

        // Если текущий фрейм полностью переслан — сбрасываем флаг и ищем следующий заголовок
        if (m_payload_remaining == 0)
        {
            m_header_parsed = false;
            // Цикл продолжится: если в m_in_buf ещё есть данные, они пойдут на парсинг следующего <len>
        }
        else
        {
            // Пейлоад ещё не закончился, но m_in_buf опустел → ждём следующего push()
            break;
        }
    }
}

} /* namespace GHelpers */
} /* namespace App */
