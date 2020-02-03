#include "capsrv_util.hpp"

#include <ctime>

namespace ams::capsrv::util {

    Result TimestampToCalendarTime(TimeCalendarTime *datetime, u64 timestamp) {
        TimeLocationName locName;
        R_TRY(timeGetDeviceLocationName(&locName));

        TimeZoneRule rule;
        R_TRY(timeLoadTimeZoneRule(&locName, &rule));

        TimeCalendarAdditionalInfo info;
        R_TRY(timeToCalendarTime(&rule, timestamp, datetime, &info));

        return ResultSuccess();
    }

    u64 DateTimeToTimestamp(DateTime datetime) {
        std::tm tm;/* = {
            .tm_year =  datetime.year - 1900,
            .tm_mon =   datetime.month - 1,
            .tm_mday =  datetime.day,
            .tm_hour =  datetime.hour,
            .tm_min =   datetime.minute,
            .tm_sec =   datetime.second
        }*/
        return std::mktime(&tm);
    }

    u8 aes128Key[0x10] = 
    {
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
    };

    Result aes128DecryptU64(u64 *programId, u8* src) {
        Aes128Context ctx;
        u64 tmp[2];
        aes128ContextCreate(&ctx, aes128Key, false);
        aes128DecryptBlock(&ctx, tmp, src); 

        R_UNLESS(tmp[1] == 0, 0x16ce);
        *programId = tmp[0];
        return ResultSuccess();
    }

    void aes128EncryptU64(u64* dst, u64 programId) {
        Aes128Context ctx;
        u64 tmp[2];
        tmp[0] = programId;
        aes128ContextCreate(&ctx, aes128Key, true);
        aes128EncryptBlock(&ctx, dst, tmp); 
    }
    
    u8 aes256Key[0x20] =
    {
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
    };

}
