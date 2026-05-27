/*
 * Storage.hpp
 *
 *  Created on: 18 мая 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_CONFIG_COMMANDLINE_STORAGE_HPP_
#define SRC_CONFIG_COMMANDLINE_STORAGE_HPP_

#include <string>
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
 * @brief
 * Конфигурация
 */
struct Storage
{
    Mode mode = Mode::UNDEFINED;
    std::string tcp_endpoint{};          // host:port
    std::string dbus_socket = "/run/dbus/system_bus_socket";
    std::string listen_socket{};         // Local Unix socket (только client)
    bool debug = false;
};


} /* namespace Config */
} /* namespace App */

#endif /* SRC_CONFIG_COMMANDLINE_STORAGE_HPP_ */
