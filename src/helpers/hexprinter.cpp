/*
 * hexprinter.c
 *
 *  Created on: 11 мар. 2026 г.
 *      Author: mastersan
 */

#include "hexprinter.hpp"

#include "logger.hpp"

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

namespace App
{
namespace GHelpers
{

void hexprint(
        std::string prefix,
        const void * data,
        size_t size
)
{
    const uint8_t *ptr = (const uint8_t *)data;
    size_t offset = 0;

    // 16 * 3 (hex + пробел) + 1 (разделитель после 8-го байта) + 1 (\0)
    char hex_buf[16 * 3 + 1 + 1];
    char ascii_buf[16 + 1];

    if (data == NULL || size == 0)
    {
        return;
    }

    while (offset < size)
    {
        size_t hex_len = 0;
        size_t idx;
        size_t ofs;
        for (ofs = 0; ofs < 16 && ((idx = offset + ofs) < size); ++ofs)
        {
            const uint8_t val = ptr[idx];

            hex_len += snprintf(
                    hex_buf + hex_len, sizeof(hex_buf) - hex_len,
                    "%02X ",
                    val
            );
            ascii_buf[ofs] = isprint(val) ? (char)val : '.';

            /* Визуальный разделитель строго после 8-го байта */
            if (ofs == 7)
            {
                hex_buf[hex_len++] = ' ';
                hex_buf[hex_len] = '\0';
            }
        }
        hex_buf[hex_len] = '\0';
        ascii_buf[ofs] = '\0';

        APPLOG_DEBUG(
                "%s%08zx | %-49s | %s",
                prefix.c_str(),
                offset,
                hex_buf,
                ascii_buf
        );

        offset += 16;
    }
}

} /* namespace GHelpers */
} /* namespace App */

