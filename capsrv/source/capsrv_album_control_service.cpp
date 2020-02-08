#include "capsrv_album_control_service.hpp"

#include "impl/capsrv_fs.hpp"
#include "impl/capsrv_manager.hpp"
#include "logger.hpp"

namespace ams::capsrv {

//Result AlbumControlService::CaptureRawImage();
//Result AlbumControlService::CaptureRawImageWithTimeout();

Result AlbumControlService::SetShimLibraryVersion(u64 version, u64 aruid) {
	WriteLogFile("Control", "SetShimLibraryVersion: version(%ld), aruid(%ld)", version, aruid);
	return ResultSuccess();
}

//Result AlbumControlService::RequestTakingScreenShot();
//Result AlbumControlService::RequestTakingScreenShotWithTimeout();
//Result AlbumControlService::NotifyTakingScreenShotRefused();

Result AlbumControlService::NotifyAlbumStorageIsAvailable(StorageId storage) {
	WriteLogFile("Control", "NotifyAlbumStorageIsAvailable: storage(%hhd)", storage);
	return impl::MountAlbum(storage);
}

Result AlbumControlService::NotifyAlbumStorageIsUnavailable(StorageId storage) {
	WriteLogFile("Control", "NotifyAlbumStorageIsUnavailable: storage(%hhd)", storage);
	return impl::UnmountAlbum(storage);
}

Result AlbumControlService::RegisterAppletResourceUserId(u64 aruid) {
	WriteLogFile("Control", "RegisterAppletResourceUserId: storage(%ld)", aruid);
	return ResultSuccess();
}

Result AlbumControlService::UnregisterAppletResourceUserId(u64 aruid) {
	WriteLogFile("Control", "UnregisterAppletResourceUserId: storage(%ld)", aruid);
	return ResultSuccess();
}

Result AlbumControlService::GetApplicationIdFromAruid(sf::Out<u64> appId, u64 aruid) {
	WriteLogFile("Control", "GetApplicationIdFromAruid: storage(%ld)", aruid);
	return ResultSuccess();
}

Result AlbumControlService::CheckApplicationIdRegistered(u64 appId) {
	WriteLogFile("Control", "CheckApplicationIdRegistered: applicationId(%lx)", appId);
	return ResultSuccess();
}

Result AlbumControlService::GenerateCurrentAlbumFileId(sf::Out<FileId> out, u64 appId, ContentType type) {
	WriteLogFile("Control", "GenerateCurrentAlbumFileId: applicationId(%lx), type(%hhd)", appId, type);
	return impl::GenerateCurrentAlbumFileId(out.GetPointer(), appId, type);
}

Result AlbumControlService::GenerateApplicationAlbumEntry(sf::Out<ApplicationEntry> out, u64 appId, ContentType type) {
	WriteLogFile("Control", "GenerateApplicationAlbumEntry: applicationId(%lx), type(%hhd)", appId, type);
	return ResultSuccess();
}

Result AlbumControlService::SaveAlbumScreenShotFile(sf::InBuffer buffer, const FileId &fileId) {
	WriteLogFile("Control", "SaveAlbumScreenShotFile: bufferSize(%ld), fileId(%s)", buffer.GetSize(), fileId.AsString().c_str());
	return ResultSuccess();
}

Result AlbumControlService::SaveAlbumScreenShotFileEx(sf::InNonSecureBuffer buffer, const FileId &fileId, u64 unk0, u64 unk1, u64 unk2) {
	WriteLogFile("Control", "SaveAlbumScreenShotFileEx: bufferSize(%ld), fileId(%s), %ld:%ld:%ld", buffer.GetSize(), fileId.AsString().c_str(), unk0, unk1, unk2);
	return ResultSuccess();
}

Result AlbumControlService::SetOverlayScreenShotThumbnailData(sf::InNonSecureBuffer buffer, const FileId &fileId) {
	WriteLogFile("Control", "SetOverlayScreenShotThumbnailData: bufferSize(%ld), fileId(%s)", buffer.GetSize(), fileId.AsString().c_str());
	return ResultSuccess();
}

Result AlbumControlService::SetOverlayMovieThumbnailData(sf::InNonSecureBuffer buffer, const FileId &fileId) {
	WriteLogFile("Control", "SetOverlayMovieThumbnailData: bufferSize(%ld), fileId(%s)", buffer.GetSize(), fileId.AsString().c_str());
	return ResultSuccess();
}

Result AlbumControlService::OpenControlSession(sf::Out<std::shared_ptr<ControlSession>> out, const sf::ClientAppletResourceUserId &aruid) {
	WriteLogFile("Control", "OpenControlSession: aruid(%ld)", aruid.GetValue());
	auto session = std::make_shared<ControlSession>();
	out.SetValue(std::move(session));
	return ResultSuccess();
}
} // namespace ams::capsrv