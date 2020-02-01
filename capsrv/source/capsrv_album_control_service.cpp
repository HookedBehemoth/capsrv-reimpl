#include "capsrv_album_control_service.hpp"

#include "logger.hpp"

namespace ams::capsrv {

    //Result AlbumControlService::CaptureRawImage();
    //Result AlbumControlService::CaptureRawImageWithTimeout();

    Result AlbumControlService::SetShimLibraryVersion(u64 version, u64 aruid) {
        WriteLogFile(LogType_Info, "SetShimLibraryVersion: version(%ld), aruid(%ld)", version, aruid);
        return ResultSuccess();
    }

    //Result AlbumControlService::RequestTakingScreenShot();
    //Result AlbumControlService::RequestTakingScreenShotWithTimeout();
    //Result AlbumControlService::NotifyTakingScreenShotRefused();

    Result AlbumControlService::NotifyAlbumStorageIsAvailable(StorageId storage) {
        WriteLogFile(LogType_Info, "NotifyAlbumStorageIsAvailable: storage(%hhd)", storage);
        return ResultSuccess();
    }

    Result AlbumControlService::NotifyAlbumStorageIsUnavailable(StorageId storage) {
        WriteLogFile(LogType_Info, "NotifyAlbumStorageIsUnavailable: storage(%hhd)", storage);
        return ResultSuccess();
    }

    Result AlbumControlService::RegisterAppletResourceUserId(u64 aruid) {
        WriteLogFile(LogType_Info, "RegisterAppletResourceUserId: storage(%ld)", aruid);
        return ResultSuccess();
    }

    Result AlbumControlService::UnregisterAppletResourceUserId(u64 aruid) {
        WriteLogFile(LogType_Info, "UnregisterAppletResourceUserId: storage(%ld)", aruid);
        return ResultSuccess();
    }

    Result AlbumControlService::GetApplicationIdFromAruid(sf::Out<u64> appId, u64 aruid) {
        WriteLogFile(LogType_Info, "GetApplicationIdFromAruid: storage(%ld)", aruid);
        return ResultSuccess();
    }

    Result AlbumControlService::CheckApplicationIdRegistered(u64 appId) {
        WriteLogFile(LogType_Info, "CheckApplicationIdRegistered: applicationId(%ld)", appId);
        return ResultSuccess();
    }

    Result AlbumControlService::GenerateCurrentAlbumFileId(sf::Out<FileId> out, u64 appId, ContentType type) {
        WriteLogFile(LogType_Info, "GenerateCurrentAlbumFileId: applicationId(%ld), type(%hhd)", appId, type);
        return ResultSuccess();
    }

    Result AlbumControlService::GenerateApplicationAlbumEntry(sf::Out<CapsApplicationAlbumEntry> out, u64 appId, ContentType type) {
        WriteLogFile(LogType_Info, "GenerateApplicationAlbumEntry: applicationId(%ld), type(%hhd)", appId, type);
        return ResultSuccess();
    }
    
    Result AlbumControlService::SaveAlbumScreenShotFile(sf::InBuffer buffer, const FileId &fileId) {
        WriteLogFile(LogType_Info, "SaveAlbumScreenShotFile: bufferSize(%ld), applicationId(%s)", buffer.GetSize(), fileId.AsString().c_str());
        return ResultSuccess();
    }

    Result AlbumControlService::SaveAlbumScreenShotFileEx(sf::InNonSecureBuffer buffer, const FileId &fileId, u64 unk0, u64 unk1, u64 unk2) {
        WriteLogFile(LogType_Info, "SaveAlbumScreenShotFileEx: bufferSize(%ld), applicationId(%s), %ld:%ld:%ld", buffer.GetSize(), fileId.AsString().c_str(), unk0, unk1, unk2);
        return ResultSuccess();
    }
    
    Result AlbumControlService::SetOverlayScreenShotThumbnailData(sf::InNonSecureBuffer buffer, const FileId &fileId) {
        WriteLogFile(LogType_Info, "SetOverlayScreenShotThumbnailData: bufferSize(%ld), applicationId(%s)", buffer.GetSize(), fileId.AsString().c_str());
        return ResultSuccess();
    }
    
    Result AlbumControlService::SetOverlayMovieThumbnailData(sf::InNonSecureBuffer buffer, const FileId &fileId) {
        WriteLogFile(LogType_Info, "SetOverlayMovieThumbnailData: bufferSize(%ld), applicationId(%s)", buffer.GetSize(), fileId.AsString().c_str());
        return ResultSuccess();
    }

    Result AlbumControlService::OpenControlSession(sf::Out<std::shared_ptr<ControlSession>> out, const sf::ClientProcessId &client_pid, u64 aruid) {
        WriteLogFile(LogType_Info, "OpenControlSession: pid(%ld), aruid(%ld)", client_pid.GetValue(), aruid);
        auto session = std::make_shared<ControlSession>();
        out.SetValue(std::move(session));
        return ResultSuccess();
    }
}