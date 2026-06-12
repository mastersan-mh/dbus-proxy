
#pragma once

#include "config_storage/Storage.hpp"
#include "common/DbusFd.hpp"
#include "common/ChanId.hpp"

#include <map>

namespace App
{
namespace Common
{

void relay(
        const Config::Storage& cfg,
        const std::map<DbusFd, Common::Types::ChanId>& channels_conf,
        int tcp_fd
);

} /* namespace Common */
} /* namespace App */
