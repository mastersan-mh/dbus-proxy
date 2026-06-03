
#ifndef SRC_COMMON_RELAY_HPP_
#define SRC_COMMON_RELAY_HPP_

#include "config_commandline/Storage.hpp"

namespace App
{
namespace Common
{

void relay(
        const Config::Storage& cfg,
        int dbus_fd,
        int tcp_fd
);

} /* namespace Common */
} /* namespace App */

#endif /* SRC_COMMON_RELAY_HPP_ */
