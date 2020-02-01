#include "capsrv_album_application_service.hpp"

namespace ams::capsrv {

    Result AlbumApplicationService::SetShimLibraryVersion(const sf::ClientAppletResourceUserId &aruid, u64 version) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList0AafeAruidDeprecated(sf::OutBuffer buffer, const sf::ClientAppletResourceUserId &aruid, ContentType type, u64 startTimestamp, u64 endTimestamp) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::DeleteAlbumFileByAruid(const sf::ClientAppletResourceUserId &aruid, ContentType type, const ApplicationFileEntry &entry) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileSizeByAruid(sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, const ApplicationFileEntry &entry) {
        return ResultSuccess();
    }
    
    Result AlbumApplicationService::DeleteAlbumFileByAruidForDebug(const sf::ClientAppletResourceUserId &aruid, const ApplicationFileEntry &entry) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::LoadAlbumScreenShotImageByAruid(sf::Out<LoadAlbumScreenShotImageOutputForApplication> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const sf::ClientAppletResourceUserId &aruid, const ApplicationFileEntry &entry, const CapsScreenShotDecodeOption &opts) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::LoadAlbumScreenShotThumbnailImageByAruid(sf::Out<LoadAlbumScreenShotImageOutputForApplication> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const sf::ClientAppletResourceUserId &aruid, const ApplicationFileEntry &entry, const CapsScreenShotDecodeOption &opts) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::PrecheckToCreateContentsByAruid(const sf::ClientAppletResourceUserId &aruid, ContentType type, u64 unk) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList1AafeAruidDeprecated(sf::OutBuffer buffer, sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, ContentType type, const DateTime &start, const DateTime &end) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList2AafeUidAruidDeprecated(sf::OutBuffer buffer, sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, ContentType type, const DateTime &start, const DateTime &end, AccountUid uid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList3AaeAruid(sf::OutBuffer buffer, sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, ContentType type, const DateTime &start, const DateTime &end) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList4AaeUidAruid(sf::OutBuffer buffer, sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, ContentType type, const DateTime &start, const DateTime &end, AccountUid uid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::OpenAccessorSessionForApplication(sf::Out<std::shared_ptr<AccessorApplicationSession>> out, const sf::ClientAppletResourceUserId &aruid, const CapsApplicationAlbumFileEntry &entry) {
        auto session = std::make_shared<AccessorApplicationSession>();
        out.SetValue(std::move(session));
        return ResultSuccess();
    }

}