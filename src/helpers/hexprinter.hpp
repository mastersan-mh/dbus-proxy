/*
 * hexprinter.h
 *
 *  Created on: 11 мар. 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_HELPERS_HEXPRINTER_HPP_
#define SRC_HELPERS_HEXPRINTER_HPP_

#include <string>
#include <stddef.h>

namespace App
{
namespace GHelpers
{

/**
 * @brief Вывод в лог hex-дампа
 * в формате
 * <addr> | X0 X1 X2 X3 X4 X5 X6 X7 X0 X1 X2 X3 X4 X5 X6 X7 | abcdefghabcdefgh
 */
void hexprint(
        std::string prefix,
        const void * data,
        size_t size
);

} /* namespace GHelpers */
} /* namespace App */

#endif /* SRC_HELPERS_HEXPRINTER_HPP_ */
