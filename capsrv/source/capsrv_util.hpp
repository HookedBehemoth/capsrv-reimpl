#pragma once
#include "capsrv_types.hpp"

#include <vapours/results.hpp>
#include <switch.h>

namespace ams::capsrv::util {

    Result TimestampToCalendarTime(TimeCalendarTime *datetime, const u64 timestamp);

}
