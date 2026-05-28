
#ifndef SRC_COMMON_RELAY_HPP_
#define SRC_COMMON_RELAY_HPP_

#include "config_commandline/Storage.hpp"
#include "common/ChanId.hpp"

#include <map>

namespace App
{
namespace Common
{

using DbusFd = int;

void relay(
        const Config::Storage& cfg,
        const std::map<DbusFd, Common::Types::ChanId>& channels_conf,
        int tcp_fd
);

} /* namespace Common */
} /* namespace App */

#endif /* SRC_COMMON_RELAY_HPP_ */
