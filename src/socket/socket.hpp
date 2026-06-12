/*
 * socket.hpp
 *
 *  Created on: 1 jun. 2026 г.
 *      Author: mastersan
 */

#pragma once

#include "buffer/WriteBuffer.hpp"

#include <fcntl.h>

namespace App
{
namespace Socket
{

static inline
void setnonblock(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

enum class RecvErr
{
    OK,
    AGAIN,
    ERROR,
};

RecvErr try_recv(
        int fd,
        void* buf,
        size_t capacity,
        size_t& size
);

enum class SendErr
{
    OK,
    AGAIN,
    ERROR,
};

SendErr try_send(
        int fd,
        Buffer::WriteBuffer& wbuf
);

} /* namespace Socket */
} /* namespace App */
