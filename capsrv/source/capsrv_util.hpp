#pragma once
#include <stratosphere.hpp>
#include "capsrv_types.hpp"

namespace ams::capsrv::util {

    Result TimestampToCalendarTime(TimeCalendarTime *datetime, u64 timestamp);
    u64 DateTimeToTimestamp(DateTime datetime);

    Result aes128DecryptU64(u64 *programId, u8* src);
    void aes128EncryptU64(u64* dst, u64 programId);

}
