/*
 * parser.cpp
 *
 *  Created on: 18 мая 2026 г.
 *      Author: mastersan
 */

#include "config_commandline/parser.hpp"
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
            "      --tcp ADDR           TCP endpoint (host:port)\n"
            "                           Listened by server.\n"
            "                           Point to connect for client.\n"
            "      --dbus-socket PATH   D-Bus socket path (default: /run/dbus/system_bus_socket)\n"
            "      --listen-socket PATH Local Unix socket to create (client only)\n"
            "      --spoof-uid UID      UID to spoof in AUTH EXTERNAL (client only)\n"
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
        OPTION_VAL__TCP,
        OPTION_VAL__DBUS_SOCKET,
        OPTION_VAL__LISTEN_SOCKET,
    };

    const char * prog = argv[0];
    Config::Storage cfg;

    static int opt_flag = 0;
    static struct option long_options[] =
    {
            {"help"         , no_argument      , &opt_flag, OPTION_VAL__HELP},
            {"debug"        , no_argument      , &opt_flag, OPTION_VAL__DEBUG},
            {"mode"         , required_argument, &opt_flag, OPTION_VAL__MODE},
            {"tcp"          , required_argument, &opt_flag, OPTION_VAL__TCP},
            {"dbus-socket"  , required_argument, &opt_flag, OPTION_VAL__DBUS_SOCKET},
            {"listen-socket", required_argument, &opt_flag, OPTION_VAL__LISTEN_SOCKET},
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
                    case OPTION_VAL__TCP:
                    {
                        cfg.tcp_endpoint = optarg;
                        break;
                    }
                    case OPTION_VAL__DBUS_SOCKET:
                    {
                        cfg.dbus_socket = optarg;
                        break;
                    }
                    case OPTION_VAL__LISTEN_SOCKET:
                    {
                        cfg.listen_socket = optarg;
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

    if(cfg.tcp_endpoint.empty())
    {
        APPLOG_ERROR("Error: --tcp required.");
        P_print_usage(prog);
        exit(1);
    }

    if(cfg.mode == Mode::UNDEFINED)
    {
        APPLOG_ERROR("Error: mode must be set.");
        P_print_usage(prog);
        exit(1);
    }

    if(cfg.mode == Mode::CLIENT && cfg.listen_socket.empty())
    {
        APPLOG_ERROR("Error: client requires --listen-socket.");
        P_print_usage(prog);
        exit(1);
    }

    return cfg;
}

} /* namespace Config */
} /* namespace App */
