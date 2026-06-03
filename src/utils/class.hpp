/*
 * class.hpp
 *
 */

#ifndef SRC_UTILS_CLASS_HPP_
#define SRC_UTILS_CLASS_HPP_

namespace App
{

/**
 * @brief Отключить копирование класса
 */
#define CLASS_NO_COPY(name) \
        name(const name&) = delete; \
        name& operator=(const name&) = delete

#define CLASS_NO_MOVE(name) \
        name(name&&) = delete; \
        name& operator=(name&&) = delete

#define CLASS_DEFAULT_COPY(name) \
        name(const name&) = default; \
        name& operator=(const name&) = default

#define CLASS_DEFAULT_MOVE(name) \
        name(name&&) = default; \
        name& operator=(name&&) = default

} /* namespace App */

#endif /* SRC_UTILS_CLASS_HPP_ */
