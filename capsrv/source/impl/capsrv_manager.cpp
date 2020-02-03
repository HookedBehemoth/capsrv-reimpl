#include "capsrv_manager.hpp"
#include "capsrv_fs.hpp"
#include "../capsrv_util.hpp"

#include <mutex>

namespace ams::capsrv::impl {

    namespace {
        struct {
            u64 lastTimestamp = 0;
            u32 id = 0;
        } idGenerator;
        os::Mutex g_mutex;
    }

    Result InitializeAlbumAccessor() {
        return ResultSuccess();
    }

    void FinalizeAlbumAccessor() {

    }

    Result GenerateCurrentAlbumFileId(FileId *fileId, u64 appId, ContentType type) {
        StorageId storage;
        R_TRY(impl::GetAutoSavingStorage(&storage));

        u64 timestamp;
        R_TRY(timeGetCurrentTime(TimeType_Default, &timestamp));
        
        if (idGenerator.lastTimestamp == timestamp && idGenerator.id < 99) {
            idGenerator.id ++;
        } else {
            idGenerator.lastTimestamp = timestamp;
            idGenerator.id = 0;
        }

        TimeCalendarTime datetime;
        util::TimestampToCalendarTime(&datetime, timestamp);

        fileId->applicationId = appId;
        fileId->datetime.year =   datetime.year;
        fileId->datetime.month =  datetime.month;
        fileId->datetime.day =    datetime.day;
        fileId->datetime.hour =   datetime.hour;
        fileId->datetime.minute = datetime.minute;
        fileId->datetime.second = datetime.second;
        fileId->storage = storage;
        fileId->type = type;
        return ResultSuccess();
    }

    Result GetAlbumFileCount(u64* outCount, StorageId storageId) {
        u64 count = 0;
        Result res = 0;
        {
            std::scoped_lock lk(g_mutex);

        }
        if (res.IsSuccess() && outCount)
            *outCount = count;
        return res;
    }

    Result GetAlbumFileList(void* ptr, u64 size, u64* outCount, StorageId storageId) {
        u64 count = 0;
        Result res = 0;
        {
            std::scoped_lock lk(g_mutex);

        }
        if (res.IsSuccess() && outCount)
            *outCount = count;
        return res;
    }

}