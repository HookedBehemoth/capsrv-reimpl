#include "capsrv_controller.hpp"

#include <mutex>

#include "../capsrv_util.hpp"
#include "../capsrv_crypto.hpp"
#include "capsrv_fs.hpp"

namespace ams::capsrv::control {

namespace {

struct {
	u64 lastTimestamp = 0;
	u32 id = 0;
} idGenerator;

struct Resource {
	bool used;
	u64 aruid;
	u64 appId;
	u64 version;
	u64 key[4];
} resources[4];

} // namespace

Result SetShimLibraryVersion(u64 version, u64 aruid) {
	for (Resource &res : resources) {
		if (!res.used || res.aruid != aruid)
			continue;
		R_UNLESS(version == 1, 0x10ce);
		R_UNLESS(res.version != version, ResultSuccess());
		R_UNLESS(res.version == 0, 0xece);
		res.version = version;
		return ResultSuccess();
	}
	return 0x66cce;
}

Result RegisterAppletResourceUserId(u64 aruid, u64 appId) {
	for (Resource &res : resources) {
		if (res.used && res.aruid != aruid)
			continue;
		res.used = true;
		res.version = 0;
		res.aruid = aruid;
		res.appId = appId;
		randomGet(res.key, sizeof(res.key));
		return ResultSuccess();
	}
	return 0x668ce;
}

Result UnregisterAppletResourceUserId(u64 aruid, u64 appId) {
	for (Resource &res : resources) {
		if (!res.used || res.aruid != aruid)
			continue;
		res.used = false;
		res.aruid = 0;
		res.appId = 0;
		res.version = 0;
		res.key[0] = 0;
		res.key[1] = 0;
		res.key[2] = 0;
		res.key[3] = 0;
	}
	return ResultSuccess();
}

Result GetApplicationIdFromAruid(u64 *appId, u64 aruid) {
	for (Resource &res : resources) {
		if (!res.used || res.aruid != aruid)
			continue;
		*appId = res.appId;
		return ResultSuccess();
	}
	return 0x66cce;
}

Result CheckApplicationIdRegistered(u64 appId) {
	for (Resource &res : resources) {
		if (res.used && res.appId == appId)
			return ResultSuccess();
	}
	return 0x66cce;
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

Result GenerateApplicationAlbumEntry(ApplicationEntry *appEntry, const Entry &entry, u64 appId) {
	Result rc = ResultSuccess();
	Resource *res = nullptr;
	for (Resource &tmp : resources) {
		if (tmp.used && tmp.appId == appId) {
			res = &tmp;
			break;
		}
	}
	if (!res)
		return 0x66cce;

	ApplicationEntry tmp;
	if (res->version == 0) {
		crypto::aes256::EncryptV0(&tmp, &entry, (u8 *)res->key);
	} else {
		rc = crypto::aes256::EncryptV1(&tmp, &entry, res->version);
	}
	if (rc.IsSuccess())
		*appEntry = tmp;
	return rc;
}

} // namespace ams::capsrv::control