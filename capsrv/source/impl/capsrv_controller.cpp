#include "capsrv_controller.hpp"

#include <mutex>

#include "../capsrv_crypto.hpp"
#include "capsrv_manager.hpp"
#include "../capsrv_time.hpp"

#define FIND_RESOURCE(by, cmp)       \
    for (Resource & res : resources) \
        if (res.used && res.by == cmp)

namespace ams::capsrv::control {

    namespace {

        struct Resource {
            bool used;
            u64 aruid;
            u64 appId;
            u64 version;
            u64 key[4];
        } resources[4];

        os::Mutex g_mutex;

        inline Result GetAlbumEntryFromApplicationAlbumEntryV0Impl(Entry *out, const ApplicationEntry *src, u64 appId, const u64 key[4]) {
            Entry tmp;
            crypto::aes256::DecryptV0(&tmp, src, (const u8 *)key);
            R_UNLESS(tmp.fileId.applicationId == appId, capsrv::ResultInvalidApplicationId());
            *out = tmp;
            return ResultSuccess();
        }

        inline Result GetAlbumEntryFromApplicationAlbumEntryV1Impl(Entry *out, const ApplicationEntry *src, u64 appId) {
            Entry tmp;
            R_TRY(crypto::aes256::DecryptV1(&tmp, src, appId));
            R_TRY(tmp.fileId.Verify());
            *out = tmp;
            return ResultSuccess();
        }

    }

    Result RegisterAppletResourceUserId(u64 aruid, u64 appId) {
        std::scoped_lock lk(g_mutex);
        /* Find unused or own resource. */
        for (Resource &res : resources) {
            if (!res.used || res.aruid == aruid) {
                res.used = true;
                res.version = 0;
                res.aruid = aruid;
                res.appId = appId;
                /* Generate random AES-256 key. */
                randomGet(res.key, sizeof(res.key));
                return ResultSuccess();
            }
        }
        return capsrv::ResultTooManyApplicationsRegistered();
    }

    Result SetShimLibraryVersion(u64 version, u64 aruid) {
        std::scoped_lock lk(g_mutex);
        /* Find own resource. */
        FIND_RESOURCE(aruid, aruid) {
            /* Only allow setting version to 1. */
            R_UNLESS(version == 1, capsrv::ResultOutOfRange());
            R_UNLESS(res.version != version, ResultSuccess());
            R_UNLESS(res.version == 0, capsrv::ResultInvalidState());
            res.version = version;
            return ResultSuccess();
        }
        return capsrv::ResultApplicationNotRegistered();
    }

    Result UnregisterAppletResourceUserId(u64 aruid, u64 appId) {
        std::scoped_lock lk(g_mutex);
        /* Find own resource. */
        FIND_RESOURCE(aruid, aruid) {
            /* Clear field. */
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
        std::scoped_lock lk(g_mutex);
        /* Find own resource. */
        FIND_RESOURCE(aruid, aruid) {
            /* Return appId. */
            *appId = res.appId;
            return ResultSuccess();
        }
        return capsrv::ResultApplicationNotRegistered();
    }

    Result CheckApplicationIdRegistered(u64 applicationId) {
        std::scoped_lock lk(g_mutex);
        /* Find own resource. */
        FIND_RESOURCE(appId, applicationId) {
            /* Report success. */
            return ResultSuccess();
        }
        return capsrv::ResultApplicationNotRegistered();
    }

    Result GenerateApplicationAlbumEntry(ApplicationEntry *appEntry, const Entry &entry, u64 applicationId) {
        std::scoped_lock lk(g_mutex);
        /* Find own resource. */
        FIND_RESOURCE(appId, applicationId) {
            /* Generate ApplicationEntry. */
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

    Result EncryptAafeBuffer(ApplicationFileEntry *appFileEntry, u64 count, u64 aruid) {
        std::scoped_lock lk(g_mutex);
        /* Find own resource. */
        FIND_RESOURCE(aruid, aruid) {
            for (u64 i = 0; i < count; i++) {
                /* Generate ApplicationEntry. */
                if (res.version == 0) {
                    crypto::aes256::EncryptV0(&appFileEntry[i].entry, (const Entry *)&appFileEntry[i].entry, (u8 *)res.key);
                } else {
                    R_TRY(crypto::aes256::EncryptV1(&appFileEntry[i].entry, (const Entry *)&appFileEntry[i].entry, res.version));
                }
            }
            return ResultSuccess();
        }
        return capsrv::ResultApplicationNotRegistered();
    }

    Result EncryptAaeBuffer(ApplicationEntry *appEntry, u64 count, u64 aruid) {
        std::scoped_lock lk(g_mutex);
        /* Find own resource. */
        FIND_RESOURCE(aruid, aruid) {
            for (u64 i = 0; i < count; i++) {
                /* Generate ApplicationEntry. */
                if (res.version == 0) {
                    crypto::aes256::EncryptV0(&appEntry[i], (const Entry *)&appEntry[i], (u8 *)res.key);
                } else {
                    R_TRY(crypto::aes256::EncryptV1(&appEntry[i], (const Entry *)&appEntry[i], res.version));
                }
            }
            return ResultSuccess();
        }
        return capsrv::ResultApplicationNotRegistered();
    }

    Result GetAlbumEntryFromApplicationAlbumEntry(Entry *out, const ApplicationEntry *src, u64 applicationId) {
        std::scoped_lock lk(g_mutex);
        /* Find own resource. */
        FIND_RESOURCE(appId, applicationId) {
            /* Generate Entry. */
            if (res.version == 0) {
                return GetAlbumEntryFromApplicationAlbumEntryV0Impl(out, src, res.appId, res.key);
            } else {
                return GetAlbumEntryFromApplicationAlbumEntryV1Impl(out, src, res.appId);
            }
        }
        return capsrv::ResultApplicationNotRegistered();
    }

    Result GetAlbumEntryFromApplicationAlbumEntryAruid(Entry *out, const ApplicationEntry *src, u64 aruid) {
        std::scoped_lock lk(g_mutex);
        /* Find own resource. */
        FIND_RESOURCE(aruid, aruid) {
            /* Generate Entry. */
            if (res.version == 0) {
                return GetAlbumEntryFromApplicationAlbumEntryV0Impl(out, src, res.appId, res.key);
            } else {
                return GetAlbumEntryFromApplicationAlbumEntryV1Impl(out, src, res.appId);
            }
        }
        return capsrv::ResultApplicationNotRegistered();
    }

}
