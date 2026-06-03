/*
 * socket.hpp
 *
 *  Created on: 1 июн. 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_HELPERS_SOCKET_HPP_
#define SRC_HELPERS_SOCKET_HPP_

#include "helpers/WriteBuffer.hpp"

#include <fcntl.h>

namespace App
{
namespace GHelpers
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
        WriteBuffer& wbuf
);

} /* namespace Socket */
} /* namespace GHelpers */
} /* namespace App */

#endif /* SRC_HELPERS_SOCKET_HPP_ */
