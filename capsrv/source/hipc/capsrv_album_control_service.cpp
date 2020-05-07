#include "capsrv_album_control_service.hpp"

#include "../server/capsrv_album_object.hpp"

namespace ams::capsrv::server {

    Result AlbumControlService::CaptureRawImage(sf::OutNonSecureBuffer out, u64 a, u64 b, u64 c, u64 d) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }
    
    Result AlbumControlService::CaptureRawImageWithTimeout(sf::OutNonSecureBuffer out, u64 a, u64 b, u64 c, u64 d, u64 e) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result AlbumControlService::SetShimLibraryVersion(u64 version, u64 aruid) {
        std::scoped_lock lk(g_AlbumMutex);

        return g_AlbumErrorConverter.ConvertError(g_ApplicationAlbumManager.SetShimLibraryVersion(version, aruid));
    }

    Result AlbumControlService::RequestTakingScreenShot(u64 a, u64 b) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result AlbumControlService::RequestTakingScreenShotWithTimeout(u64 a, u64 b, u64 c) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result AlbumControlService::NotifyTakingScreenShotRefused(u64 a) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result AlbumControlService::NotifyAlbumStorageIsAvailable(StorageId storage_id) {
        std::scoped_lock lk(g_AlbumMutex);

        Result rc = g_AlbumManager.UnmountImageDirectory(storage_id);
        if (rc.IsSuccess()) rc = g_AlbumManager.RefreshCache(storage_id);
        return g_AlbumErrorConverter.ConvertError(rc);
    }

    Result AlbumControlService::NotifyAlbumStorageIsUnavailable(StorageId storage_id) {
        std::scoped_lock lk(g_AlbumMutex);

        return g_AlbumErrorConverter.ConvertError(g_AlbumManager.CloseImageDirectory(storage_id));
    }

    Result AlbumControlService::RegisterAppletResourceUserId(u64 aruid, u64 application_id) {
        return g_AlbumErrorConverter.ConvertError(g_ApplicationAlbumManager.RegisterAppletResourceUserId(aruid, application_id));
    }

    Result AlbumControlService::UnregisterAppletResourceUserId(u64 aruid, u64 application_id) {
        g_ApplicationAlbumManager.UnregisterAppletResourceUserId(aruid, application_id);
        return ResultSuccess();
    }

    Result AlbumControlService::GetApplicationIdFromAruid(sf::Out<u64> out, u64 aruid) {
        u64 application_id = 0;
        R_TRY(g_AlbumErrorConverter.ConvertError(g_ApplicationAlbumManager.GetApplicationIdFromAruid(&application_id, aruid)));

        out.SetValue(application_id);
        return ResultSuccess();
    }

    Result AlbumControlService::CheckApplicationIdRegistered(u64 application_id) {
        return g_AlbumErrorConverter.ConvertError(g_ApplicationAlbumManager.CheckApplicationIdRegistered(application_id));
    }

    Result AlbumControlService::GenerateCurrentAlbumFileId(sf::Out<AlbumFileId> out, u64 application_id, ContentType type) {
        AlbumFileId file_id = {};
        R_TRY(g_AlbumErrorConverter.ConvertError(g_AlbumFileIdGenerator.GenerateFileId(&file_id, application_id, type, &g_AlbumManager)));

        out.SetValue(file_id);
        return ResultSuccess();
    }

    Result AlbumControlService::GenerateApplicationAlbumEntry(sf::Out<ApplicationAlbumEntry> out, const AlbumEntry &entry, u64 application_id) {
        ApplicationAlbumEntry app_entry = {};
        R_TRY(g_AlbumErrorConverter.ConvertError(g_ApplicationAlbumManager.GenerateApplicationAlbumEntry(&app_entry, entry, application_id)));

        out.SetValue(app_entry);
        return ResultSuccess();
    }

    Result AlbumControlService::SaveAlbumScreenShotFile(sf::InBuffer buffer, const AlbumFileId &file_id) {
        std::scoped_lock lk(g_AlbumMutex);

        return g_AlbumErrorConverter.ConvertError(g_AlbumManager.SaveScreenshotFile(buffer.GetPointer(), buffer.GetSize(), file_id));
    }

    Result AlbumControlService::SaveAlbumScreenShotFileEx(sf::InNonSecureBuffer buffer, const AlbumFileId &file_id, u64 unk0, u64 unk1, u64 unk2) {
        std::scoped_lock lk(g_AlbumMutex);

        return g_AlbumErrorConverter.ConvertError(g_AlbumManager.SaveScreenshotFile(buffer.GetPointer(), buffer.GetSize(), file_id, unk0, unk1, unk2));
    }

    Result AlbumControlService::SetOverlayScreenShotThumbnailData(sf::InNonSecureBuffer buffer, const AlbumFileId &file_id) {
        const u8 *src_ptr = buffer.GetPointer();
        size_t src_size = buffer.GetSize();
        R_UNLESS(src_size >= g_OverlayThumbnailHolder.GetBufferSize(), capsrv::ResultAlbumError());

        u8 *dst = g_OverlayThumbnailHolder.GetOverlayBuffer(file_id, ContentType::Screenshot);
        std::memcpy(dst, src_ptr, g_OverlayThumbnailHolder.GetBufferSize());
        g_OverlayThumbnailHolder.Signal(ContentType::Screenshot);

        return ResultSuccess();
    }

    Result AlbumControlService::SetOverlayMovieThumbnailData(sf::InNonSecureBuffer buffer, const AlbumFileId &file_id) {
        const u8 *src_ptr = buffer.GetPointer();
        size_t src_size = buffer.GetSize();
        R_UNLESS(src_size >= g_OverlayThumbnailHolder.GetBufferSize(), capsrv::ResultAlbumError());

        u8 *dst = g_OverlayThumbnailHolder.GetOverlayBuffer(file_id, ContentType::Movie);
        std::memcpy(dst, src_ptr, g_OverlayThumbnailHolder.GetBufferSize());
        g_OverlayThumbnailHolder.Signal(ContentType::Movie);

        return ResultSuccess();
    }

    Result AlbumControlService::OpenControlSession(sf::Out<std::shared_ptr<ControlSession>> out, const sf::ClientAppletResourceUserId &aruid) {
        out.SetValue(std::make_shared<ControlSession>());
        return ResultSuccess();
    }

}
