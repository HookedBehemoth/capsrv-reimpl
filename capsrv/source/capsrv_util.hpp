#pragma once
#include <stratosphere.hpp>
#include "capsrv_types.hpp"

namespace ams::capsrv::util {

    Result TimestampToCalendarTime(TimeCalendarTime *datetime, const u64 timestamp);

}
