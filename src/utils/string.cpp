/*
 * string.cpp
 *
 *  Created on: 27 мая 2026 г.
 *      Author: mastersan
 */

#include "utils/string.hpp"

namespace App
{
namespace Utils
{
namespace String
{

std::vector<std::string_view> split(const std::string& str)
{
    std::vector<std::string_view> words;
    size_t i = 0;

    while(i < str.size())
    {
        // 1. Пропускаем пробельные символы
        while(i < str.size() && std::isspace(static_cast<unsigned char>(str[i])))
        {
            ++i;
        }
        if (i >= str.size())
        {
            break;
        }

        // 2. Находим конец текущего слова
        size_t j = i;
        while (j < str.size() && !std::isspace(static_cast<unsigned char>(str[j])))
        {
            ++j;
        }

        // 3. Сохраняем view на найденный фрагмент
        words.push_back(str.substr(i, j - i));
        i = j;
    }

    return words;
}

} /* namespace String */
} /* namespace Utils */
} /* namespace App */
