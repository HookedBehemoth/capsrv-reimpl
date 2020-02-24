#pragma once
#include <stratosphere.hpp>

#include "capsrv_types.hpp"

namespace ams::capsrv::util {

    Result TimestampToCalendarTime(DateTime *datetime, u64 timestamp);
    u64 DateTimeToTimestamp(DateTime datetime);

}
