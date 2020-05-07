#include "capsrv_application_manager.hpp"

#include "capsrv_types.hpp"
#include "capsrv_crypto.hpp"
#include "server/capsrv_album_object.hpp"

namespace ams::capsrv {

    namespace {

        ALWAYS_INLINE Result GetAlbumEntryFromApplicationAlbumEntryV0Impl(AlbumEntry *out, const ApplicationAlbumEntry *src, u64 application_id, const u8 *key) {
            AlbumEntry tmp;
            crypto::application::DecryptV0(&tmp, src, key);
            R_UNLESS(tmp.fileId.application_id == application_id, capsrv::ResultAlbumInvalidApplicationId());
            *out = tmp;
            return ResultSuccess();
        }

        ALWAYS_INLINE Result GetAlbumEntryFromApplicationAlbumEntryV1Impl(AlbumEntry *out, const ApplicationAlbumEntry *src, u64 application_id) {
            AlbumEntry tmp;
            R_TRY(crypto::application::DecryptV1(&tmp, src, application_id));
            R_TRY(tmp.fileId.Verify(&server::g_AlbumSettings));
            *out = tmp;
            return ResultSuccess();
        }

    }

    ApplicationAlbumManager::ApplicationAlbumManager() : initialized(false), mutex(false), resources() {
        /* TODO */
    }

    void ApplicationAlbumManager::Initialize() {
        /* TODO. See constructor. */
        this->initialized = true;
    }
    void ApplicationAlbumManager::Exit() {
        for (auto &resource : this->resources)
            if (resource.used)
                resource = {};
        this->initialized = false;
    }

    Result ApplicationAlbumManager::RegisterAppletResourceUserId(u64 aruid, u64 application_id) {
        std::scoped_lock lk(this->mutex);
        /* Find unused or own resource. */
        for (auto &res : resources) {
            if (!res.used) {
                if (res.aruid != aruid) {
                    res.used = true;
                    res.version = 0;
                    res.aruid = aruid;
                    res.application_id = application_id;

                    /* Generate random AES-256 key. */
                    u8 zero_buffer[0x20] = {};
                    do {
                        ams::crypto::GenerateCryptographicallyRandomBytes(res.key, sizeof(res.key));
                    } while(std::memcmp(zero_buffer, res.key, sizeof(res.key)) == 0);

                    return ResultSuccess();
                } else {
                    return capsrv::ResultControlAlreadyOpen();
                }
            }
        }

        return capsrv::ResultControlResourceLimit();
    }

    void ApplicationAlbumManager::UnregisterAppletResourceUserId(u64 aruid, u64 application_id) {
        std::scoped_lock lk(this->mutex);
        /* Find own resource. */
        for (auto res : this->resources) {
            if (res.aruid != aruid)
                continue;

            /* Clear field. */
            res.used = false;
            res.aruid = 0;
            res.application_id = 0;
            res.version = 0;
            std::memset(res.key, 0, sizeof(res.key));
        }
    }

    Result ApplicationAlbumManager::GetApplicationIdFromAruid(u64 *application_id, u64 aruid) {
        std::scoped_lock lk(this->mutex);
        /* Find own resource. */
        for (auto res : this->resources) {
            if (res.aruid != aruid)
                continue;

            /* Return ApplicationId. */
            *application_id = res.application_id;
            return ResultSuccess();
        }
        return capsrv::ResultControlNotOpened();
    }

    Result ApplicationAlbumManager::CheckApplicationIdRegistered(u64 application_id) {
        std::scoped_lock lk(this->mutex);

        /* Find own resource. */
        for (auto res : this->resources) {
            if (res.application_id != application_id)
                continue;
    
            /* Report success. */
            return ResultSuccess();
        }
        return capsrv::ResultControlNotOpened();
    }

    Result ApplicationAlbumManager::SetShimLibraryVersion(u64 version, u64 aruid) {
        std::scoped_lock lk(this->mutex);

        /* Find own resource. */
        for (auto res : this->resources) {
            if (res.aruid != aruid)
                continue;

            /* Only allow setting version to 1. */
            R_UNLESS(version == 1, capsrv::ResultAlbumOutOfRange());
            R_SUCCEED_IF(res.version == version);
            R_UNLESS(res.version == 0, capsrv::ResultAlbumInvalidState());
            res.version = version;
            return ResultSuccess();
        }
        return capsrv::ResultControlNotOpened();
    }

    Result ApplicationAlbumManager::GenerateApplicationAlbumEntry(ApplicationAlbumEntry *appEntry, const AlbumEntry &entry, u64 application_id) {
        std::scoped_lock lk(this->mutex);
        /* Find own resource. */
        for (auto res : this->resources) {
            if (res.application_id != application_id)
                continue;
    
            /* Generate ApplicationAlbumEntry. */
            if (res.version == 0) {
                crypto::application::EncryptV0(appEntry, &entry, (u8 *)res.key);
            } else {
                ApplicationAlbumEntry tmp;
                R_TRY(crypto::application::EncryptV1(&tmp, &entry, res.version));
                *appEntry = tmp;
            }
            return ResultSuccess();
        }
        return capsrv::ResultControlNotOpened();
    }

    Result ApplicationAlbumManager::GetAlbumEntryFromApplicationAlbumEntry(AlbumEntry *out, const ApplicationAlbumEntry *src, u64 application_id) {
        std::scoped_lock lk(this->mutex);
        /* Find own resource. */
        for (auto res : this->resources) {
            if (res.application_id != application_id)
                continue;
    
            /* Generate Entry. */
            if (res.version == 0) {
                return GetAlbumEntryFromApplicationAlbumEntryV0Impl(out, src, res.application_id, res.key);
            } else {
                return GetAlbumEntryFromApplicationAlbumEntryV1Impl(out, src, res.application_id);
            }
        }
        return capsrv::ResultControlNotOpened();
    }
    Result ApplicationAlbumManager::GetAlbumEntryFromApplicationAlbumEntryAruid(AlbumEntry *out, const ApplicationAlbumEntry *src, u64 aruid) {
        std::scoped_lock lk(this->mutex);
        /* Find own resource. */
        for (auto res : this->resources) {
            if (res.aruid != aruid)
                continue;
    
            /* Generate Entry. */
            if (res.version == 0) {
                return GetAlbumEntryFromApplicationAlbumEntryV0Impl(out, src, res.application_id, res.key);
            } else {
                return GetAlbumEntryFromApplicationAlbumEntryV1Impl(out, src, res.application_id);
            }
        }
        return capsrv::ResultControlNotOpened();
    }

    /* TODO */
    Result ApplicationAlbumManager::EncryptAaeBuffer(ApplicationAlbumEntry *appEntry, u64 count, u64 aruid) {
        std::scoped_lock lk(this->mutex);
        /* Find own resource. */
        for (auto res : this->resources) {
            if (res.aruid != aruid)
                continue;
    
            for (u64 i = 0; i < count; i++) {
                /* Generate ApplicationAlbumEntry. */
                if (res.version == 0) {
                    crypto::application::EncryptV0(&appEntry[i], (const AlbumEntry *)&appEntry[i], res.key);
                } else {
                    R_TRY(crypto::application::EncryptV1(&appEntry[i], (const AlbumEntry *)&appEntry[i], res.version));
                }
            }
            return ResultSuccess();
        }
        return capsrv::ResultControlNotOpened();
    }

    Result ApplicationAlbumManager::EncryptAafeBuffer(ApplicationAlbumFileEntry *appFileEntry, u64 count, u64 aruid) {
        std::scoped_lock lk(this->mutex);
        /* Find own resource. */
        for (auto res : this->resources) {
            if (res.aruid != aruid)
                continue;

            for (u64 i = 0; i < count; i++) {
                /* Generate ApplicationAlbumEntry. */
                if (res.version == 0) {
                    crypto::application::EncryptV0(&appFileEntry[i].entry, (const AlbumEntry *)&appFileEntry[i].entry, (u8 *)res.key);
                } else {
                    R_TRY(crypto::application::EncryptV1(&appFileEntry[i].entry, (const AlbumEntry *)&appFileEntry[i].entry, res.version));
                }
            }
            return ResultSuccess();
        }
        return capsrv::ResultControlNotOpened();
    }

}
