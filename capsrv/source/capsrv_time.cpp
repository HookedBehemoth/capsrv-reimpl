#include "capsrv_time.hpp"

#include <switch.h>

namespace ams::capsrv::time {

    namespace {
        TimeZoneRule rule;
    }

    Result Initialize() {
        TimeLocationName locName;
        R_TRY(timeGetDeviceLocationName(&locName));

        R_TRY(timeLoadTimeZoneRule(&locName, &rule));

        return ResultSuccess();
    }

    Result TimestampToCalendarTime(DateTime *datetime, u64 timestamp) {
        TimeCalendarAdditionalInfo info;
        return timeToCalendarTime(&rule, timestamp, (TimeCalendarTime *)datetime, &info);
    }

    Result DateTimeToTimestamp(u64 *timestamp, DateTime datetime) {
        s32 count;
        return timeToPosixTime(&rule, (TimeCalendarTime *)&datetime, timestamp, 1, &count);
    }

}
