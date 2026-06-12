/*
 * logger.hpp
 *
 *  Created on: 18 мая 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_LOGGER_HPP_
#define SRC_LOGGER_HPP_

#include <inttypes.h>
#include <stdio.h>

namespace App
{

#define APPLOG_PRINTF(format, ...) \
    printf(format "\n", ##__VA_ARGS__)

#define APPLOG_ERROR(format, ...) \
    printf("Error: " format "\n", ##__VA_ARGS__)

#define APPLOG_WARN(format, ...) \
    printf("Warning: " format "\n", ##__VA_ARGS__)

#define APPLOG_INFO(format, ...) \
    printf("Info: " format "\n", ##__VA_ARGS__)

#define APPLOG_DEBUG(format, ...) \
    printf("Debug: " format "\n", ##__VA_ARGS__)

} /* namespace App */

#endif /* SRC_LOGGER_HPP_ */
