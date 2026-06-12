
#include "socket/socket.hpp"

#include <stdint.h>
#include <sys/socket.h>
#include <errno.h>

namespace App
{
namespace Socket
{

RecvErr try_recv(
        int fd,
        void* buf,
        size_t capacity,
        size_t& size
)
{
    const ssize_t buf_size = recv(fd, buf, capacity, 0);
    if(buf_size < 0)
    {
        const int err = errno;
        if(err == EAGAIN || err == EWOULDBLOCK)
        {
            return RecvErr::AGAIN;
        }
        return RecvErr::ERROR;
    }
    if(buf_size == 0)
    {
        return RecvErr::END_OF_STREAM;
    }
    size = buf_size;
    return RecvErr::OK;
}

SendErr try_send(
        int fd,
        Buffer::WriteBuffer& wbuf
) noexcept
{
    while(!wbuf.empty())
    {
        const ssize_t written =
                send(fd, wbuf.data(), wbuf.size(), MSG_NOSIGNAL);
        if(written < 0)
        {
            const int err = errno;
            if (err == EAGAIN || err == EWOULDBLOCK)
            {
                return SendErr::AGAIN;
            }
            return SendErr::ERROR;
        }
        if (written == 0)
        {
            return SendErr::ERROR;
        }
        wbuf.strip_begin(written);
    }
    return SendErr::OK;
};

} /* namespace Socket */
} /* namespace App */

