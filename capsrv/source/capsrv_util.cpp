#include "capsrv_util.hpp"

namespace ams::capsrv::util {

    Result TimestampToCalendarTime(TimeCalendarTime *datetime, const u64 timestamp) {
        TimeLocationName locName;
        R_TRY(timeGetDeviceLocationName(&locName));

        TimeZoneRule rule;
        R_TRY(timeLoadTimeZoneRule(&locName, &rule));

        TimeCalendarAdditionalInfo info;
        R_TRY(timeToCalendarTime(&rule, timestamp, datetime, &info));

        return ResultSuccess();
    }

}
