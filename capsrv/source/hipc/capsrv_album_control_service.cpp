#include "capsrv_album_control_service.hpp"

#include "../impl/capsrv_controller.hpp"
#include "../impl/capsrv_manager.hpp"
#include "../impl/capsrv_overlay.hpp"
#include "../logger.hpp"

namespace ams::capsrv {

    Result AlbumControlService::CaptureRawImage(sf::OutNonSecureBuffer out, u64 a, u64 b, u64 c, u64 d) {
        WriteLogFile("Control", "CaptureRawImage: buf_size: %ld, [%ld, %ld, %ld, %ld]", out.GetSize(), a, b, c, d);
        return ResultSuccess();
    }
    
    Result AlbumControlService::CaptureRawImageWithTimeout(sf::OutNonSecureBuffer out, u64 a, u64 b, u64 c, u64 d, u64 e) {
        WriteLogFile("Control", "CaptureRawImageWithTimeout: buf_size: %ld, [%ld, %ld, %ld, %ld, %ld]", out.GetSize(), a, b, c, d, e);
        return ResultSuccess();
    }

    Result AlbumControlService::SetShimLibraryVersion(u64 version, u64 aruid) {
        WriteLogFile("Control", "SetShimLibraryVersion: version(%ld), aruid(%ld)", version, aruid);
        return control::SetShimLibraryVersion(version, aruid);
    }

    Result AlbumControlService::RequestTakingScreenShot(u64 a, u64 b) {
        WriteLogFile("Control", "RequestTakingScreenShot: [%ld, %ld]", a, b);
        return ResultSuccess();
    }

    Result AlbumControlService::RequestTakingScreenShotWithTimeout(u64 a, u64 b, u64 c) {
        WriteLogFile("Control", "RequestTakingScreenShotWithTimeout: [%ld, %ld, %ld]", a, b, c);
        return ResultSuccess();
    }

    Result AlbumControlService::NotifyTakingScreenShotRefused(u64 a) {
        WriteLogFile("Control", "NotifyTakingScreenShotRefused: [%ld]", a);
        return ResultSuccess();
    }

    Result AlbumControlService::NotifyAlbumStorageIsAvailable(StorageId storage) {
        WriteLogFile("Control", "NotifyAlbumStorageIsAvailable: storage(%hhd)", storage);
        return impl::MountAlbum(storage);
    }

    Result AlbumControlService::NotifyAlbumStorageIsUnavailable(StorageId storage) {
        WriteLogFile("Control", "NotifyAlbumStorageIsUnavailable: storage(%hhd)", storage);
        return impl::UnmountAlbum(storage);
    }

    Result AlbumControlService::RegisterAppletResourceUserId(u64 aruid, u64 appId) {
        WriteLogFile("Control", "RegisterAppletResourceUserId: storage(%ld)", aruid);
        return control::RegisterAppletResourceUserId(aruid, appId);
    }

    Result AlbumControlService::UnregisterAppletResourceUserId(u64 aruid, u64 appId) {
        WriteLogFile("Control", "UnregisterAppletResourceUserId: storage(%ld)", aruid);
        return control::UnregisterAppletResourceUserId(aruid, appId);
    }

    Result AlbumControlService::GetApplicationIdFromAruid(sf::Out<u64> appId, u64 aruid) {
        WriteLogFile("Control", "GetApplicationIdFromAruid: storage(%ld)", aruid);
        return control::GetApplicationIdFromAruid(appId.GetPointer(), aruid);
    }

    Result AlbumControlService::CheckApplicationIdRegistered(u64 appId) {
        WriteLogFile("Control", "CheckApplicationIdRegistered: applicationId(%lx)", appId);
        return control::CheckApplicationIdRegistered(appId);
    }

    Result AlbumControlService::GenerateCurrentAlbumFileId(sf::Out<FileId> out, u64 appId, ContentType type) {
        WriteLogFile("Control", "GenerateCurrentAlbumFileId: applicationId(%lx), type(%hhd)", appId, type);
        return control::GenerateCurrentAlbumFileId(out.GetPointer(), appId, type);
    }

    Result AlbumControlService::GenerateApplicationAlbumEntry(sf::Out<ApplicationEntry> out, const Entry &entry, u64 appId) {
        WriteLogFile("Control", "GenerateApplicationAlbumEntry: applicationId(%lx), fileId(%s)", appId, entry.AsString());
        return control::GenerateApplicationAlbumEntry(out.GetPointer(), entry, appId);
    }

    Result AlbumControlService::SaveAlbumScreenShotFile(sf::InBuffer buffer, const FileId &fileId) {
        WriteLogFile("Control", "SaveAlbumScreenShotFile: bufferSize(%ld), fileId(%s)", buffer.GetSize(), fileId.AsString());
        return impl::SaveAlbumScreenShotFile(buffer.GetPointer(), buffer.GetSize(), fileId);
    }

    Result AlbumControlService::SaveAlbumScreenShotFileEx(sf::InNonSecureBuffer buffer, const FileId &fileId, u64 unk0, u64 unk1, u64 unk2) {
        WriteLogFile("Control", "SaveAlbumScreenShotFileEx: bufferSize(%ld), fileId(%s), %ld:%ld:%ld", buffer.GetSize(), fileId.AsString(), unk0, unk1, unk2);
        return impl::SaveAlbumScreenShotFile(buffer.GetPointer(), buffer.GetSize(), fileId);
    }

    Result AlbumControlService::SetOverlayScreenShotThumbnailData(sf::InNonSecureBuffer buffer, const FileId &fileId) {
        WriteLogFile("Control", "SetOverlayScreenShotThumbnailData: bufferSize(%ld), fileId(%s)", buffer.GetSize(), fileId.AsString());
        return ovl::SetOverlayThumbnailData(buffer.GetPointer(), buffer.GetSize(), fileId, false);
    }

    Result AlbumControlService::SetOverlayMovieThumbnailData(sf::InNonSecureBuffer buffer, const FileId &fileId) {
        WriteLogFile("Control", "SetOverlayMovieThumbnailData: bufferSize(%ld), fileId(%s)", buffer.GetSize(), fileId.AsString());
        return ovl::SetOverlayThumbnailData(buffer.GetPointer(), buffer.GetSize(), fileId, true);
    }

    Result AlbumControlService::OpenControlSession(sf::Out<std::shared_ptr<ControlSession>> out, const sf::ClientAppletResourceUserId &aruid) {
        WriteLogFile("Control", "OpenControlSession: aruid(%ld)", aruid.GetValue());
        auto session = std::make_shared<ControlSession>();
        out.SetValue(std::move(session));
        return ResultSuccess();
    }

}
