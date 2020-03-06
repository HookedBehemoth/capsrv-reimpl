#pragma once
#include <stratosphere.hpp>

#include "capsrv_types.hpp"

namespace ams::capsrv::time {

    Result TimestampToCalendarTime(DateTime *datetime, u64 timestamp);
    u64 DateTimeToTimestamp(DateTime datetime);

}
