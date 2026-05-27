/*
 * io.hpp
 *
 *  Created on: 18 мая 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_UTILS_IO_HPP_
#define SRC_UTILS_IO_HPP_

#include <string>
#include <type_traits>
#include <stdlib.h>
#include <stdint.h>

namespace App
{
namespace Utils
{
namespace io
{

bool read_exact(int fd, void * buf, size_t n);

template<typename Type>
bool read_exact(int fd, Type& data)
{
    static_assert(
            std::is_trivially_copyable_v<Type>,
            "read_exact requires trivially copyable type"
    );

    return read_exact(fd, reinterpret_cast<uint8_t*>(&data), sizeof(Type));
}

bool write_exact(int fd, const void * data, size_t n);

static inline
bool write_exact(int fd, const std::string& str)
{
    return write_exact(fd, str.data(), str.size());
}

template<typename Type>
bool write_exact(int fd, const Type& data)
{
    static_assert(
            std::is_trivially_copyable_v<Type>,
            "write_exact requires trivially copyable type"
    );

    return write_exact(fd, reinterpret_cast<const uint8_t*>(&data), sizeof(Type));
}

} /* namespace io */
} /* namespace Utils */
} /* namespace App */

#endif /* SRC_IO_HPP_ */
