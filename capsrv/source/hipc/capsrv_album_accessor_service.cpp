#include "capsrv_album_accessor_service.hpp"

#include "../server/capsrv_album_object.hpp"
#include "../capsrv_environment.hpp"

namespace ams::capsrv::server {

    Result AlbumAccessorService::GetAlbumFileCount(sf::Out<u64> out, StorageId storage_id) {
        u64 count = 0;
        {
            std::scoped_lock lk(g_AlbumMutex);
            R_TRY(g_AlbumErrorConverter.ConvertError(g_AlbumManager.CountEntries(&count, storage_id, ContentTypeFlag_Normal)));
        }
        out.SetValue(count);

        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumFileList(sf::Out<u64> out, sf::OutArray<AlbumEntry> entries, StorageId storage_id) {
        u64 count = 0;
        {
            std::scoped_lock lk(g_AlbumMutex);
            R_TRY(g_AlbumErrorConverter.ConvertError(g_AlbumManager.ListEntries(&count, entries.GetPointer(), entries.GetSize(), storage_id, ContentTypeFlag_Normal)));
        }
        out.SetValue(count);

        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumFile(sf::OutBuffer image, sf::Out<u64> out, const AlbumFileId &file_id) {
        size_t size;
        {
            std::scoped_lock lk(g_AlbumMutex);
            R_TRY(g_AlbumErrorConverter.ConvertError(g_AlbumManager.LoadJpeg(&size, image.GetPointer(), image.GetSize(), file_id)));
        }
        out.SetValue(size);

        return ResultSuccess();
    }

    Result AlbumAccessorService::DeleteAlbumFile(const AlbumFileId &file_id) {
        std::scoped_lock lk(g_AlbumMutex);

        return g_AlbumErrorConverter.ConvertError(g_AlbumManager.DeleteFile(file_id));
    }

    Result AlbumAccessorService::StorageCopyAlbumFile(StorageId dst, const AlbumFileId &file_id) {
        std::scoped_lock lk(g_AlbumMutex);
    
        return g_AlbumErrorConverter.ConvertError(g_AlbumManager.CopyFile(file_id, dst));
    }

    Result AlbumAccessorService::IsAlbumMounted(sf::Out<bool> out, StorageId storage_id) {
        std::scoped_lock lk(g_AlbumMutex);

        out.SetValue(g_AlbumManager.MountImageDirectory(storage_id).IsSuccess());

        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumUsage(sf::Out<CapsAlbumUsage2> usage, StorageId storage_id) {
        std::scoped_lock lk(g_AlbumMutex);

        return g_AlbumErrorConverter.ConvertError(g_AlbumManager.GetAlbumUsage(&usage->usages[0], &usage->usages[1], 1, storage_id, ContentTypeFlag_ScreenShot));
    }

    Result AlbumAccessorService::GetAlbumFileSize(sf::Out<s64> out, const AlbumFileId &file_id) {
        s64 size;
        {
            std::scoped_lock lk(g_AlbumMutex);
            R_TRY(g_AlbumErrorConverter.ConvertError(g_AlbumManager.GetFileSize(&size, file_id)));
        }
        out.SetValue(size);

        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumFileThumbnail(sf::OutBuffer image, sf::Out<u64> out, const AlbumFileId &file_id) {
        std::scoped_lock lk(g_AlbumMutex);

        size_t size;
        Result rc = g_AlbumManager.LoadJpegThumbnail(&size, image.GetPointer(), image.GetSize(), file_id);
        out.SetValue(size);

        return g_AlbumErrorConverter.ConvertError(rc);
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImage(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const AlbumFileId &file_id) {
        ON_SCOPE_EXIT { std::memset(work.GetPointer(), 0, work.GetSize()); };
        Dimensions dims = {};
        CapsScreenShotAttribute attr = {};
        CapsScreenShotDecodeOption opts = {};
        {
            std::scoped_lock lk(g_AlbumMutex);
            /* TODO */
        }
        height.SetValue(dims.height);
        width.SetValue(dims.width);
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImage(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const AlbumFileId &file_id) {
        ON_SCOPE_EXIT { std::memset(work.GetPointer(), 0, work.GetSize()); };
        Dimensions dims = {};
        CapsScreenShotAttribute attr = {};
        CapsScreenShotDecodeOption opts = {};
        {
            std::scoped_lock lk(g_AlbumMutex);
            /* TODO */
        }
        height.SetValue(dims.height);
        width.SetValue(dims.width);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumEntryFromApplicationAlbumEntry(sf::Out<AlbumEntry> out, const ApplicationAlbumEntry &app_entry, u64 application_id) {
        AlbumEntry entry;
        R_TRY(g_AlbumErrorConverter.ConvertError(g_ApplicationAlbumManager.GetAlbumEntryFromApplicationAlbumEntry(&entry, &app_entry, application_id)));
        out.SetValue(entry);

        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImageEx(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const AlbumFileId &file_id, const CapsScreenShotDecodeOption &opts) {
        ON_SCOPE_EXIT { std::memset(work.GetPointer(), 0, work.GetSize()); };
        Dimensions dims = {};
        CapsScreenShotAttribute attr = {};
        {
            std::scoped_lock lk(g_AlbumMutex);
            /* TODO */
        }
        height.SetValue(dims.height);
        width.SetValue(dims.width);
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImageEx(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const AlbumFileId &file_id, const CapsScreenShotDecodeOption &opts) {
        ON_SCOPE_EXIT { std::memset(work.GetPointer(), 0, work.GetSize()); };
        Dimensions dims = {};
        CapsScreenShotAttribute attr = {};
        {
            std::scoped_lock lk(g_AlbumMutex);
            /* TODO */
        }
        height.SetValue(dims.height);
        width.SetValue(dims.width);
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImageEx0(sf::Out<CapsScreenShotAttribute> out_attr, sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const AlbumFileId &file_id, const CapsScreenShotDecodeOption &opts) {
        ON_SCOPE_EXIT { std::memset(work.GetPointer(), 0, work.GetSize()); };
        Dimensions dims = {};
        CapsScreenShotAttribute attr = {};
        {
            std::scoped_lock lk(g_AlbumMutex);
            /* TODO */
        }
        height.SetValue(dims.height);
        width.SetValue(dims.width);
        out_attr.SetValue(attr);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumUsage3(sf::Out<CapsAlbumUsage3> usage, StorageId storage_id) {
        std::scoped_lock lk(g_AlbumMutex);

        return g_AlbumErrorConverter.ConvertError(g_AlbumManager.GetAlbumUsage(&usage->usages[0], &usage->usages[1], 2, storage_id, ContentTypeFlag_Normal));
    }

    Result AlbumAccessorService::GetAlbumMountResult(StorageId storage_id) {
        std::scoped_lock lk(g_AlbumMutex);

        return g_AlbumErrorConverter.ConvertError(g_AlbumManager.MountImageDirectory(storage_id));
    }

    Result AlbumAccessorService::GetAlbumUsage16(sf::Out<AlbumUsage16> usage, StorageId storage_id, u8 flags) {
        std::scoped_lock lk(g_AlbumMutex);

        return g_AlbumErrorConverter.ConvertError(g_AlbumManager.GetAlbumUsage(&usage->usages[0], &usage->usages[1], 0xf, storage_id, flags));
    }

    Result AlbumAccessorService::GetMinMaxAppletId(sf::OutNonSecureBuffer minMax, sf::Out<bool> success) {
        if (minMax.GetSize() != 0) {
            u64 *arr = (u64 *)minMax.GetPointer();
            arr[0] = 0x100000000001000;
            arr[1] = 0x100000000001fff;
            success.SetValue(true);
        }
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumFileCountEx0(sf::Out<u64> out, StorageId storage_id, u8 flags) {
        u64 count = 0;
        {
            std::scoped_lock lk(g_AlbumMutex);
            R_TRY(g_AlbumErrorConverter.ConvertError(g_AlbumManager.CountEntries(&count, storage_id, flags)));
        }
        out.SetValue(count);

        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumFileListEx0(sf::OutArray<AlbumEntry> entries, sf::Out<u64> out, StorageId storage_id, u8 flags) {
        u64 count = 0;
        {
            std::scoped_lock lk(g_AlbumMutex);
            R_TRY(g_AlbumErrorConverter.ConvertError(g_AlbumManager.ListEntries(&count, entries.GetPointer(), entries.GetSize(), storage_id, flags)));
        }
        out.SetValue(count);

        return ResultSuccess();
    }

    Result AlbumAccessorService::SaveEditedScreenShot(sf::InBuffer buf0, sf::InBuffer buf1, sf::Out<AlbumEntry> out, u64 a, u64 b, u64 c, u64 d, u64 e, u64 f, u64 g) {
        AMS_ABORT("Not implemneted");
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetLastOverlayScreenShotThumbnail(sf::OutBuffer buffer, sf::Out<AlbumFileId> out, sf::Out<u64> out_size) {
        size_t size;
        R_TRY(g_AlbumErrorConverter.ConvertError(g_OverlayThumbnailHolder.GetLastOverlayThumbnail(out.GetPointer(), &size, buffer.GetPointer(), buffer.GetSize(), ContentType::Screenshot)));
        out_size.SetValue(size);

        return ResultSuccess();
    }

    Result AlbumAccessorService::GetLastOverlayMovieThumbnail(sf::OutBuffer buffer, sf::Out<AlbumFileId> out, sf::Out<u64> out_size) {
        size_t size;
        R_TRY(g_AlbumErrorConverter.ConvertError(g_OverlayThumbnailHolder.GetLastOverlayThumbnail(out.GetPointer(), &size, buffer.GetPointer(), buffer.GetSize(), ContentType::Movie)));
        out_size.SetValue(size);

        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAutoSavingStorage(sf::Out<StorageId> out) {
        std::scoped_lock lk(g_AlbumMutex);

        return g_AlbumErrorConverter.ConvertError(g_AlbumFileIdGenerator.GetAutoSavingStorage(out.GetPointer(), ContentType::Screenshot, &g_AlbumManager));
    }

    Result AlbumAccessorService::GetRequiredStorageSpaceSizeToCopyAll(sf::Out<u64> out, StorageId dst, StorageId src) {
        std::scoped_lock lk(g_AlbumMutex);

        size_t size;
        R_TRY(g_AlbumErrorConverter.ConvertError(g_AlbumManager.GetRequiredStorageSpaceSizeToCopyAll(&size, src, dst)));
        out.SetValue(size);

        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImageEx0(sf::Out<CapsScreenShotAttribute> out_attr, sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const AlbumFileId &file_id, const CapsScreenShotDecodeOption &opts) {
        ON_SCOPE_EXIT { std::memset(work.GetPointer(), 0, work.GetSize()); };
        Dimensions dims = {};
        CapsScreenShotAttribute attr = {};
        {
            std::scoped_lock lk(g_AlbumMutex);
            /* TODO */
        }
        height.SetValue(dims.height);
        width.SetValue(dims.width);
        out_attr.SetValue(attr);
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImageEx1(sf::Out<LoadAlbumScreenShotImageOutput> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const AlbumFileId &file_id, const CapsScreenShotDecodeOption &opts) {
        ON_SCOPE_EXIT { std::memset(work.GetPointer(), 0, work.GetSize()); };
        Dimensions dims = {};
        CapsScreenShotAttribute attr = {};
        {
            std::scoped_lock lk(g_AlbumMutex);
            /* TODO */
        }
        out->height = dims.height;
        out->width = dims.width;
        out->attr = attr;
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImageEx1(sf::Out<LoadAlbumScreenShotImageOutput> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const AlbumFileId &file_id, const CapsScreenShotDecodeOption &opts) {
        ON_SCOPE_EXIT { std::memset(work.GetPointer(), 0, work.GetSize()); };
        Dimensions dims = {};
        CapsScreenShotAttribute attr = {};
        {
            std::scoped_lock lk(g_AlbumMutex);
            /* TODO */
        }
        out->height = dims.height;
        out->width = dims.width;
        out->attr = attr;
        return ResultSuccess();
    }

    Result AlbumAccessorService::ForceAlbumUnmounted(StorageId storage_id) {
        std::scoped_lock lk(g_AlbumMutex);

        return g_AlbumErrorConverter.ConvertError(g_AlbumManager.CloseImageDirectory(storage_id));
    }

    Result AlbumAccessorService::ResetAlbumMountStatus(StorageId storage_id) {
        std::scoped_lock lk(g_AlbumMutex);

        return g_AlbumErrorConverter.ConvertError(g_AlbumManager.UnmountImageDirectory(storage_id));
    }

    Result AlbumAccessorService::RefreshAlbumCache(StorageId storage_id) {
        std::scoped_lock lk(g_AlbumMutex);

        return g_AlbumErrorConverter.ConvertError(g_AlbumManager.RefreshCache(storage_id));
    }

    Result AlbumAccessorService::GetAlbumCache(sf::Out<CapsAlbumCache> cache, StorageId storage_id) {
        if (hos::GetVersion() < hos::Version_4_0_0) {
            return GetAlbumCacheEx(cache, storage_id, ContentType::Screenshot);
        } else {
            return capsrv::ResultAlbumUnsupportedVersion();
        }
    }

    Result AlbumAccessorService::GetAlbumCacheEx(sf::Out<CapsAlbumCache> cache, StorageId storage_id, const ContentType type) {
        std::scoped_lock lk(g_AlbumMutex);

        return g_AlbumErrorConverter.ConvertError(g_AlbumManager.GetCache(cache.GetPointer(), storage_id, type));
    }

    Result AlbumAccessorService::GetAlbumEntryFromApplicationAlbumEntryAruid(sf::Out<AlbumEntry> out, const sf::ClientAppletResourceUserId &aruid, const ApplicationAlbumEntry &app_entry) {
        AlbumEntry entry;
        R_TRY(g_AlbumErrorConverter.ConvertError(g_ApplicationAlbumManager.GetAlbumEntryFromApplicationAlbumEntryAruid(&entry, &app_entry, aruid.GetValue().value)));
        out.SetValue(entry);

        return ResultSuccess();
    }

    Result AlbumAccessorService::SetInternalErrorConversionEnabled(bool mode) {
        g_AlbumErrorConverter.SetMode(mode);
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadMakerNoteInfoForDebug(sf::Out<u64> out, sf::OutBuffer makernote, sf::OutBuffer work, const AlbumFileId &file_id) {
        /* Only available for debug. */
        R_UNLESS(IsDebug(), capsrv::ResultNotSupported());

        /* Only available on [6.0.0]+ so we only care for it's buffer size */
        size_t makernote_buffer_size = 0xc78;
        u8 tmp_makernote[makernote_buffer_size];

        /* Verify buffer sufficiency. */
        R_UNLESS(makernote.GetSize() >= makernote_buffer_size, capsrv::ResultAlbumReadBufferShortage());
        R_UNLESS(work.GetSize() >= 0x7d000, capsrv::ResultAlbumWorkMemoryError());

        /* Clear buffer. */
        std::memset(makernote.GetPointer(), 0, makernote.GetSize());
        std::memset(tmp_makernote, 0, makernote_buffer_size);

        /* Disable verification. */
        bool should_verify = g_AlbumSettings.SetVerifyScreenshotFiledata(false);

        /* Restore verification state and cleanup work buffer on exit. */
        ON_SCOPE_EXIT {
            std::memset(work.GetPointer(), 0, work.GetSize());
            g_AlbumSettings.SetVerifyScreenshotFiledata(should_verify);
        };

        /* Load makernote. */
        size_t size;
        R_TRY(g_AlbumErrorConverter.ConvertError(g_AlbumManager.LoadMakerNoteInfo(&size, tmp_makernote, file_id, work.GetPointer(), work.GetSize())));

        /* Copy makernote to output. */
        out.SetValue(size);
        std::memcpy(makernote.GetPointer(), tmp_makernote, size);

        return ResultSuccess();
    }

    Result AlbumAccessorService::OpenAccessorSession(sf::Out<std::shared_ptr<AccessorSession>> out, const sf::ClientAppletResourceUserId &aruid) {
        out.SetValue(std::make_shared<AccessorSession>());
        return ResultSuccess();
    }

}
