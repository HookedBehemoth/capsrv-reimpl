#include "capsrv_manager.hpp"

#include <mutex>

#include "../capsrv_util.hpp"
#include "capsrv_fs.hpp"

namespace ams::capsrv::impl {

namespace {

struct {
	u64 lastTimestamp = 0;
	u32 id = 0;
} idGenerator;
os::Mutex g_mutex;

} // namespace

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
		idGenerator.id++;
	} else {
		idGenerator.lastTimestamp = timestamp;
		idGenerator.id = 0;
	}

	R_TRY(util::TimestampToCalendarTime(&fileId->datetime, timestamp));

	fileId->applicationId = appId;
	fileId->datetime.id = idGenerator.id;
	fileId->storage = storage;
	fileId->type = type;
	return ResultSuccess();
}

} // namespace ams::capsrv::impl