#include "capsrv_file_id_generator.hpp"
#include "capsrv_util.hpp"
#include "impl/capsrv_manager.hpp"

#include <vapours/results.hpp>

namespace ams::capsrv {

    FileIdGenerator::FileIdGenerator() {
        this->lastTimestamp = 0;
        this->id = 0;
    }

    Result FileIdGenerator::GenerateFileId(FileId *fileId, const u64 appId, const ContentType type) {
        StorageId storage = impl::GetPrimaryStorage();
        Result rc = impl::MountAlbum(storage);
        if (rc.IsFailure())
            return capsrv::ResultFailedToMountImageDirectory();

        u64 timestamp;
        R_TRY(timeGetCurrentTime(TimeType_Default, &timestamp));
        
        if (this->lastTimestamp == timestamp && this->id < 99) {
            this->id ++;
        } else {
            this->lastTimestamp = timestamp;
            this->id = 0;
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

}