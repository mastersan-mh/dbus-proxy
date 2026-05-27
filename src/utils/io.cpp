/*
 * socket.cpp
 *
 *  Created on: 18 мая 2026 г.
 *      Author: mastersan
 */

#include "io.hpp"

#include <vector>

#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/wait.h>


namespace App
{
namespace Utils
{
namespace io
{

bool read_exact(int fd, void * buf, size_t n)
{
    uint8_t * ptr = reinterpret_cast<uint8_t*>(buf);
    size_t done = 0;
    while(done < n)
    {
        ssize_t r = recv(fd, ptr + done, n - done, 0);
        if(r <= 0)
        {
            return false;
        }
        done += r;
    }
    return true;
}

bool write_exact(int fd, const void * data, size_t n)
{
    const uint8_t * ptr = reinterpret_cast<const uint8_t*>(data);
    size_t done = 0;
    while(done < n)
    {
        const ssize_t w = send(fd, ptr + done, n - done, MSG_NOSIGNAL);
        if(w <= 0)
        {
            return false;
        }
        done += w;
    }
    return true;
}

} /* namespace io */
} /* namespace Utils */
} /* namespace App */
