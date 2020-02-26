#include "capsrv_controller.hpp"

#include <mutex>

#include "../capsrv_crypto.hpp"
#include "../capsrv_util.hpp"
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

        #define FIND_RESOURCE(by, cmp)\
        for (Resource &res : resources)\
            if (res.used && res.by == cmp)

        Result GetAlbumEntryFromApplicationAlbumEntryV0Impl(Entry *out, const ApplicationEntry *src, u64 appId, const u64 key[4]) {
            Entry tmp;
            crypto::aes256::DecryptV0(&tmp, src, (const u8 *)key);
            R_UNLESS(tmp.fileId.applicationId == appId, capsrv::ResultInvalidApplicationId());
            *out = tmp;
            return ResultSuccess();
        }

        Result GetAlbumEntryFromApplicationAlbumEntryV1Impl(Entry *out, const ApplicationEntry *src, u64 appId) {
            Entry tmp;
            R_TRY(crypto::aes256::DecryptV1(&tmp, src, appId));
            R_TRY(tmp.fileId.Verify());
            *out = tmp;
            return ResultSuccess();
        }

    }

    Result SetShimLibraryVersion(u64 version, u64 aruid) {
        for (Resource &res : resources) {
            if (!res.used || res.aruid != aruid)
                continue;
            R_UNLESS(version == 1, capsrv::ResultOutOfRange());
            R_UNLESS(res.version != version, ResultSuccess());
            R_UNLESS(res.version == 0, capsrv::ResultInvalidState());
            res.version = version;
            return ResultSuccess();
        }
        return capsrv::ResultApplicationNotRegistered();
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
        return capsrv::ResultTooManyApplicationsRegistered();
    }

    Result UnregisterAppletResourceUserId(u64 aruid, u64 appId) {
        FIND_RESOURCE(aruid, aruid) {
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
        FIND_RESOURCE(aruid, aruid) {
            *appId = res.appId;
            return ResultSuccess();
        }
        return capsrv::ResultApplicationNotRegistered();
    }

    Result CheckApplicationIdRegistered(u64 applicationId) {
        FIND_RESOURCE(appId, applicationId)
            return ResultSuccess();
        return capsrv::ResultApplicationNotRegistered();
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

    Result GenerateApplicationAlbumEntry(ApplicationEntry *appEntry, const Entry &entry, u64 applicationId) {
        FIND_RESOURCE(appId, applicationId) {
            if (res.version == 0) {
                crypto::aes256::EncryptV0(appEntry, &entry, (u8 *)res.key);
            } else {
                ApplicationEntry tmp;
                R_TRY(crypto::aes256::EncryptV1(&tmp, &entry, res.version));
                *appEntry = tmp;
            }
            return ResultSuccess();
        }
        return capsrv::ResultApplicationNotRegistered();
    }

    Result GetAlbumEntryFromApplicationAlbumEntry(Entry *out, const ApplicationEntry *src, u64 applicationId) {
        FIND_RESOURCE(appId, applicationId) {
            if (res.version == 0) {
                return GetAlbumEntryFromApplicationAlbumEntryV0Impl(out, src, res.appId, res.key);
            } else {
                return GetAlbumEntryFromApplicationAlbumEntryV1Impl(out, src, res.appId);
            }
        }
        return capsrv::ResultApplicationNotRegistered();
    }

    Result GetAlbumEntryFromApplicationAlbumEntryAruid(Entry *out, const ApplicationEntry *src, u64 aruid) {
        FIND_RESOURCE(aruid, aruid) {
            if (res.version == 0) {
                return GetAlbumEntryFromApplicationAlbumEntryV0Impl(out, src, res.appId, res.key);
            } else {
                return GetAlbumEntryFromApplicationAlbumEntryV1Impl(out, src, res.appId);
            }
        }
        return capsrv::ResultApplicationNotRegistered();
    }

}
