#include "capsrv_album_application_service.hpp"

#include "../impl/capsrv_controller.hpp"
#include "../impl/capsrv_manager.hpp"
#include "../logger.hpp"

namespace ams::capsrv {

    Result AlbumApplicationService::SetShimLibraryVersion(u64 version, const sf::ClientAppletResourceUserId &aruid) {
        WriteLogFile("Application", "SetShimLibraryVersion: version(%ld), aruid(%ld)", version, aruid.GetValue());
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList0AafeAruidDeprecated(sf::OutBuffer buffer, const sf::ClientAppletResourceUserId &aruid, ContentType type, u64 startTimestamp, u64 endTimestamp) {
        WriteLogFile("Application", "GetAlbumFileList0AafeAruidDeprecated: bufferSize(%ld), aruid(%ld), type(%hhd), start(%ld), end(%ld)", buffer.GetSize(), aruid.GetValue(), type, startTimestamp, endTimestamp);
        return ResultSuccess();
    }

    Result AlbumApplicationService::DeleteAlbumFileByAruid(const sf::ClientAppletResourceUserId &aruid, ContentType type, const CapsApplicationAlbumFileEntry &entry) {
        WriteLogFile("Application", "DeleteAlbumFileByAruid: aruid(%ld), type(%hhd), appEntry(#)", aruid.GetValue(), type);
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileSizeByAruid(sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, const CapsApplicationAlbumFileEntry &entry) {
        WriteLogFile("Application", "GetAlbumFileSizeByAruid: aruid(%ld), appEntry(#)", aruid.GetValue());
        return ResultSuccess();
    }

    Result AlbumApplicationService::DeleteAlbumFileByAruidForDebug(const sf::ClientAppletResourceUserId &aruid, const CapsApplicationAlbumFileEntry &entry) {
        WriteLogFile("Application", "DeleteAlbumFileByAruidForDebug: aruid(%ld), appEntry(#)", aruid.GetValue());
        return ResultSuccess();
    }

    Result AlbumApplicationService::LoadAlbumScreenShotImageByAruid(sf::Out<LoadAlbumScreenShotImageOutputForApplication> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const sf::ClientAppletResourceUserId &aruid, const CapsApplicationAlbumFileEntry &entry, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile("Application", "LoadAlbumScreenShotImageByAruid: workSize(%ld), imageSize(%ld), aruid(%ld), appEntry(#), opts(#)", work.GetSize(), image.GetSize(), aruid.GetValue());
        return impl::LoadAlbumScreenShotImageByAruid(out.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), aruid.GetValue().value, entry, opts);
    }

    Result AlbumApplicationService::LoadAlbumScreenShotThumbnailImageByAruid(sf::Out<LoadAlbumScreenShotImageOutputForApplication> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const sf::ClientAppletResourceUserId &aruid, const CapsApplicationAlbumFileEntry &entry, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile("Application", "LoadAlbumScreenShotThumbnailImageByAruid: workSize(%ld), imageSize(%ld), aruid(%ld), appEntry(#), opts(#)", work.GetSize(), image.GetSize(), aruid.GetValue());
        return impl::LoadAlbumScreenShotThumbnailImageByAruid(out.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), aruid.GetValue().value, entry, opts);
    }

    Result AlbumApplicationService::PrecheckToCreateContentsByAruid(ContentType type, u64 unk, const sf::ClientAppletResourceUserId &aruid) {
        WriteLogFile("Application", "PrecheckToCreateContentsByAruid: type(%hhd), unk(%ld), aruid(%ld)", type, unk, aruid.GetValue());
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList1AafeAruidDeprecated(sf::OutBuffer buffer, sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, ContentType type, const DateTime &start, const DateTime &end) {
        WriteLogFile("Application", "GetAlbumFileList1AafeAruidDeprecated: bufferSize(%ld), aruid(%ld), type(%hhd), start(%s), end(%s)", buffer.GetSize(), aruid.GetValue(), type, start.AsString(), end.AsString());
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList2AafeUidAruidDeprecated(sf::OutBuffer buffer, sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, ContentType type, const DateTime &start, const DateTime &end, AccountUid uid) {
        WriteLogFile("Application", "GetAlbumFileList2AafeUidAruidDeprecated: bufferSize(%ld), aruid(%ld), type(%hhd), start(%s), end(%s), uuid(%ld)", buffer.GetSize(), aruid.GetValue(), type, start.AsString(), end.AsString(), uid);
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList3AaeAruid(sf::OutBuffer buffer, sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, ContentType type, const DateTime &start, const DateTime &end) {
        WriteLogFile("Application", "GetAlbumFileList3AaeAruid: bufferSize(%ld), aruid(%ld), type(%hhd), start(%s), end(%s)", buffer.GetSize(), aruid.GetValue(), type, start.AsString(), end.AsString());
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList4AaeUidAruid(sf::OutBuffer buffer, sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, ContentType type, const DateTime &start, const DateTime &end, AccountUid uid) {
        WriteLogFile("Application", "GetAlbumFileList2AafeUidAruidDeprecated: bufferSize(%ld), aruid(%ld), type(%hhd), start(%s), end(%s), uuid(%ld)", buffer.GetSize(), aruid.GetValue(), type, start.AsString(), end.AsString(), uid);
        return ResultSuccess();
    }

    Result AlbumApplicationService::OpenAccessorSessionForApplication(sf::Out<std::shared_ptr<AccessorApplicationSession>> out, const sf::ClientAppletResourceUserId &aruid, const CapsApplicationAlbumFileEntry &entry) {
        WriteLogFile("Accessor", "OpenAccessorSession: aruid(%ld), entry(#)", aruid.GetValue());
        auto session = std::make_shared<AccessorApplicationSession>();
        out.SetValue(std::move(session));
        return ResultSuccess();
    }

}
