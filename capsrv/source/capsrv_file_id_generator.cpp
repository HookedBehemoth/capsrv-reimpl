#include "capsrv_file_id_generator.hpp"

namespace ams::capsrv {

    void AlbumFileIdGenerator::Initialize() {
        this->last_timestamp = 0;
        this->counter = 0;
    }

    void AlbumFileIdGenerator::Exit() {}

    Result AlbumFileIdGenerator::GetAutoSavingStorage(StorageId *out, ContentType type, AlbumManager *manager) {
        if (IsNormal(type)) {
            /* Get default storage. */
            SetSysPrimaryAlbumStorage setsys_storage;
            R_ABORT_UNLESS(setsysGetPrimaryAlbumStorage(&setsys_storage));
            if (setsys_storage == SetSysPrimaryAlbumStorage_SdCard) {
                /* Try to mount sd card. */
                if (R_SUCCEEDED(manager->MountImageDirectory(StorageId::Sd))) {
                    *out = StorageId::Sd;
                    return ResultSuccess();
                }
            }
            /* Fallback to nand. */
            if (R_SUCCEEDED(manager->MountImageDirectory(StorageId::Nand))) {
                *out = StorageId::Nand;
                return ResultSuccess();
            }
        } else {
            /* Try to mount sd card. */
            if (R_SUCCEEDED(manager->MountImageDirectory(StorageId::Sd))) {
                *out = StorageId::Sd;
                return ResultSuccess();
            }
            /* Check if we are allowed to write extra files to nand. */
            R_UNLESS(manager->settings->AllowSaveExtraMoviesOnNand(), capsrv::ResultAlbumIsNotMounted());
            /* Fallback to nand. */
            if (R_SUCCEEDED(manager->MountImageDirectory(StorageId::Nand))) {
                *out = StorageId::Nand;
                return ResultSuccess();
            }
        }
        return capsrv::ResultAlbumIsNotMounted();
    }

    Result AlbumFileIdGenerator::GenerateFileId(AlbumFileId *out, u64 application_id, ContentType type, AlbumManager *manager) {
        /* Get default save destination. */
        StorageId storage_id;
        R_TRY(this->GetAutoSavingStorage(&storage_id, type, manager));

        /* Get timestamp. */
        u64 timestamp;
        R_TRY(timeGetCurrentTime(TimeType_Default, &timestamp));

        /* Increment counter if timestamp matches. */
        if (this->last_timestamp == timestamp && this->counter < 99) {
            this->counter++;
        } else {
            this->last_timestamp = timestamp;
            this->counter = 0;
        }

        /* Get DateTime. */
        R_TRY(timeToCalendarTimeWithMyRule(timestamp, (TimeCalendarTime *)&out->date_time, nullptr));

        out->application_id = application_id;
        out->date_time.id = this->counter;
        out->storage_id = storage_id;
        out->type = type;
        return ResultSuccess();
    }

}
