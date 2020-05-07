#include "capsrv_album_application_service.hpp"

#include "../server/capsrv_album_object.hpp"

namespace ams::capsrv::server {

    Result AlbumApplicationService::SetShimLibraryVersion(u64 version, const sf::ClientAppletResourceUserId &aruid) {
        std::scoped_lock lk(g_AlbumMutex);

        return g_AlbumErrorConverter.ConvertError(g_ApplicationAlbumManager.SetShimLibraryVersion(version, aruid.GetValue().value));
    }

    Result AlbumApplicationService::GetAlbumFileList0AafeAruidDeprecated(sf::OutBuffer buffer, const sf::ClientAppletResourceUserId &aruid, ContentType type, u64 startTimestamp, u64 endTimestamp) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result AlbumApplicationService::DeleteAlbumFileByAruid(const sf::ClientAppletResourceUserId &aruid, ContentType type, const ApplicationAlbumFileEntry &app_file_entry) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileSizeByAruid(sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, const ApplicationAlbumFileEntry &app_file_entry) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result AlbumApplicationService::DeleteAlbumFileByAruidForDebug(const sf::ClientAppletResourceUserId &aruid, const ApplicationAlbumFileEntry &app_file_entry) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result AlbumApplicationService::LoadAlbumScreenShotImageByAruid(sf::Out<LoadAlbumScreenShotImageOutputForApplication> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const sf::ClientAppletResourceUserId &aruid, const ApplicationAlbumFileEntry &app_file_entry, const CapsScreenShotDecodeOption &opts) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result AlbumApplicationService::LoadAlbumScreenShotThumbnailImageByAruid(sf::Out<LoadAlbumScreenShotImageOutputForApplication> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const sf::ClientAppletResourceUserId &aruid, const ApplicationAlbumFileEntry &app_file_entry, const CapsScreenShotDecodeOption &opts) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result AlbumApplicationService::PrecheckToCreateContentsByAruid(ContentType type, u64 size, const sf::ClientAppletResourceUserId &aruid) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList1AafeAruidDeprecated(sf::OutBuffer buffer, sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, ContentType type, AlbumDateTime start, AlbumDateTime end) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList2AafeUidAruidDeprecated(sf::OutBuffer buffer, sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, ContentType type, AlbumDateTime start, AlbumDateTime end, const AccountUid &uid) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList3AaeAruid(sf::OutBuffer buffer, sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, ContentType type, AlbumDateTime start, AlbumDateTime end) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList4AaeUidAruid(sf::OutBuffer buffer, sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, ContentType type, AlbumDateTime start, AlbumDateTime end, const AccountUid &uid) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result AlbumApplicationService::OpenAccessorSessionForApplication(sf::Out<std::shared_ptr<AccessorApplicationSession>> out, const sf::ClientAppletResourceUserId &aruid, const ApplicationAlbumFileEntry &app_file_entry) {
        out.SetValue(std::make_shared<AccessorApplicationSession>());
        return ResultSuccess();
    }

}
