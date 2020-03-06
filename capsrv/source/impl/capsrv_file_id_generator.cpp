#include "capsrv_file_id_generator.hpp"

#include "../capsrv_time.hpp"
#include "capsrv_manager.hpp"

namespace ams::capsrv::impl {

    namespace {

        u64 lastTimestamp = 0;
        u32 id = 0;

    }

    Result GenerateCurrentAlbumFileId(FileId *fileId, u64 appId, ContentType type) {
        /* Get StorageId. */
        StorageId storage;
        R_TRY(impl::GetAutoSavingStorage(&storage));

        /* Get timestamp. */
        u64 timestamp;
        R_TRY(timeGetCurrentTime(TimeType_Default, &timestamp));

        /* Increment counter if timestamp matches. */
        if (lastTimestamp == timestamp && id < 99) {
            id++;
        } else {
            lastTimestamp = timestamp;
            id = 0;
        }

        /* Get DateTime. */
        R_TRY(time::TimestampToCalendarTime(&fileId->datetime, timestamp));

        fileId->applicationId = appId;
        fileId->datetime.id = id;
        fileId->storage = storage;
        fileId->type = type;
        return ResultSuccess();
    }

}
