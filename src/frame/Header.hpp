/*
 * FrameHeader.hpp
 *
 *  Created on: 8 июн. 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_FRME_HEADER_HPP_
#define SRC_FRME_HEADER_HPP_

#include "common/ChanId.hpp"

#include <stdint.h>

namespace App
{
namespace Frame
{

static_assert(sizeof(Common::Types::ChanId) == sizeof(uint8_t), "Bad ChanId size");

struct Header
{
    uint32_t payload_size; /**< Payload size, without header. */
    uint8_t channel;
    uint8_t gap0;
    uint8_t gap1;
    uint8_t gap2;
};

} /* namespace Frame */
} /* namespace App */

#endif /* SRC_FRME_HEADER_HPP_ */
