/*
 * parser.cpp
 *
 *  Created on: 18 мая 2026 г.
 *      Author: mastersan
 */

#include "config_commandline/parser.hpp"
#include "utils/string.hpp"
#include "logger.hpp"

#include <getopt.h>

namespace App
{
namespace Config
{

static
void P_print_usage(const char* prog)
{
    APPLOG_PRINTF(
            "Usage: %s --mode [server|client] [options]\n"
            "Options:\n"
            "      --help               Show this help\n"
            "      --mode MODE          'server' or 'client'\n"
            "      --addr ADDR          Address\n"
            "                           Server: IP address to listen.\n"
            "                           Client: Point to connect.\n"
            "      --port PORT          TCP port\n"
            "                           Server: Listened port.\n"
            "                           Client: Port to connect to.\n"
            "                           Default value: 5555.\n"
            "      --dbus-socket PATH   D-Bus socket path\n"
            "                           Client: socket to create an listen\n"
            "                           Server: socket to send stream\n"
            "                                   (Example: /run/dbus/system_bus_socket)\n"
            ,
            prog
    );
}

Storage parse_args(int argc, char* argv[])
{
    enum OPTION_VAL
    {
        OPTION_VAL__HELP = 1,
        OPTION_VAL__DEBUG,
        OPTION_VAL__MODE,
        OPTION_VAL__ADDR,
        OPTION_VAL__PORT,
        OPTION_VAL__DBUS_SOCKET,
    };

    const char * prog = argv[0];
    Config::Storage cfg;

    static int opt_flag = 0;
    static struct option long_options[] =
    {
            {"help"         , no_argument      , &opt_flag, OPTION_VAL__HELP},
            {"debug"        , no_argument      , &opt_flag, OPTION_VAL__DEBUG},
            {"mode"         , required_argument, &opt_flag, OPTION_VAL__MODE},
            {"addr"         , required_argument, &opt_flag, OPTION_VAL__ADDR},
            {"port"         , required_argument, &opt_flag, OPTION_VAL__PORT},
            {"dbus-socket"  , required_argument, &opt_flag, OPTION_VAL__DBUS_SOCKET},
            {nullptr, 0, nullptr, 0}
    };

    int opt;
    int opt_index = 0;
    while ((opt = getopt_long(argc, argv, "", long_options, &opt_index)) != -1)
    {
        switch(opt)
        {
            case 0:
            {
                switch(opt_flag)
                {
                    case OPTION_VAL__HELP:
                    {
                        P_print_usage(prog);
                        exit(0);
                    }
                    case OPTION_VAL__DEBUG:
                    {
                        cfg.debug = true;
                        break;
                    }
                    case OPTION_VAL__MODE:
                    {
                        if (std::string(optarg) == "client")
                        {
                            cfg.mode = Mode::CLIENT;
                        }
                        else if (std::string(optarg) == "server")
                        {
                            cfg.mode = Mode::SERVER;
                        }
                        else
                        {
                            APPLOG_ERROR("Invalid mode. Use 'server' or 'client'.");
                            P_print_usage(prog);
                            exit(1);
                        }
                        break;
                    }
                    case OPTION_VAL__ADDR:
                    {
                        cfg.addr = optarg;
                        break;
                    }
                    case OPTION_VAL__PORT:
                    {
                        try
                        {
                            uint64_t port = Utils::String::str_to_u64(optarg);
                            if(port > 0xffff)
                            {
                                throw std::overflow_error("Port value overflow");
                            }
                            cfg.port = port;
                        }
                        catch(...)
                        {
                            APPLOG_ERROR("Invalid port.");
                            P_print_usage(prog);
                            exit(1);
                        }
                        break;
                    }
                    case OPTION_VAL__DBUS_SOCKET:
                    {
                        cfg.dbus_socket = optarg;
                        break;
                    }
                    default:
                    {
                        P_print_usage(prog);
                        exit(1);
                    }
                }
                break;
            }
            case '?':
            {
                P_print_usage(prog);
                exit(1);
            }
            default:
            {
                APPLOG_ERROR("Got unknown option: %s", argv[optind]);
                P_print_usage(prog);
                exit(1);
            }
        }
    }

    if(cfg.addr.empty())
    {
        APPLOG_ERROR("Error: --addr required.");
        P_print_usage(prog);
        exit(1);
    }

    switch(cfg.mode)
    {
        case Mode::UNDEFINED:
        {
            APPLOG_ERROR("Error: mode must be set.");
            P_print_usage(prog);
            exit(1);
        }
        case(Mode::CLIENT):
        {
            if(cfg.dbus_socket.empty())
            {
                APPLOG_ERROR("Error: client requires --dbus-socket.");
                P_print_usage(prog);
                exit(1);
            }
            break;
        }
        case(Mode::SERVER):
        {
            if(cfg.dbus_socket.empty())
            {
                APPLOG_ERROR("Error: server requires --dbus-socket.");
                APPLOG_ERROR("Example: /run/dbus/system_bus_socket");
                P_print_usage(prog);
                exit(1);
            }
            break;
        }
    }

    return cfg;
}

} /* namespace Config */
} /* namespace App */
