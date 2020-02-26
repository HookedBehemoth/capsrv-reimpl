#include "capsrv_util.hpp"

#include <switch.h>

namespace ams::capsrv::util {

    Result TimestampToCalendarTime(DateTime *datetime, u64 timestamp) {
        TimeLocationName locName;
        R_TRY(timeGetDeviceLocationName(&locName));

        TimeZoneRule rule;
        R_TRY(timeLoadTimeZoneRule(&locName, &rule));

        TimeCalendarAdditionalInfo info;
        R_TRY(timeToCalendarTime(&rule, timestamp, (TimeCalendarTime *)datetime, &info));

        return ResultSuccess();
    }

    Result DateTimeToTimestamp(u64 *timestamp, DateTime datetime) {
        TimeLocationName locName;
        R_TRY(timeGetDeviceLocationName(&locName));

        TimeZoneRule rule;
        R_TRY(timeLoadTimeZoneRule(&locName, &rule));

        s32 count;
        R_TRY(timeToPosixTime(&rule, (TimeCalendarTime *)&datetime, timestamp, 1, &count));

        return ResultSuccess();
    }

}
