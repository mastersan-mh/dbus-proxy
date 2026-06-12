/*
 * Storage.hpp
 *
 *  Created on: 18 мая 2026 г.
 *      Author: mastersan
 */

#pragma once

#include "common/ChanId.hpp"

#include <string>
#include <map>
#include <filesystem>
#include <stdint.h>

namespace App
{
namespace Config
{

enum class Mode
{
    UNDEFINED,
    SERVER,
    CLIENT,
};

/**
 * @brief Configuration
 */
struct Storage
{
    bool debug = false;
    Mode mode = Mode::UNDEFINED;
    std::string addr{};
    uint16_t port = 5555;
    std::map<Common::Types::ChanId, std::filesystem::path> bus_mapping{};
};


} /* namespace Config */
} /* namespace App */

