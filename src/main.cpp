// dbus-proxy.cpp
// Исследовательский D-Bus TCP-прокси с постоянным фреймингом
// Компиляция: g++ -std=c++17 -O2 -o dbus-proxy dbus-proxy.cpp

#include "config_commandline/parser.hpp"
#include "server/server.hpp"
#include "client/client.hpp"
#include "logger.hpp"

#include <signal.h>
#include <stdexcept>

namespace App
{

static
void run(int argc, char* argv[])
{
    Config::Storage cfg = Config::parse_args(argc, argv);

    switch(cfg.mode)
    {
        case Config::Mode::UNDEFINED:
        {
            throw std::runtime_error("Invalid mode");
        }
        case Config::Mode::CLIENT:
        {
            Client::run(cfg);
            break;
        }
        case Config::Mode::SERVER:
        {
            Server::run(cfg);
            break;
        }
    }
}
} /* namespace App */


int main(int argc, char* argv[])
{
    try
    {
        App::run(argc, argv);
    }
    catch (const std::exception& e)
    {
        APPLOG_ERROR("%s", e.what());
        return 1;
    }
    return 0;
}
