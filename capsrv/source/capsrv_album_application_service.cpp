#include "capsrv_album_application_service.hpp"

namespace ams::capsrv {

    Result AlbumApplicationService::SetShimLibraryVersion(const u64 version, const u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList0AafeAruidDeprecated(sf::OutBuffer buffer, const ContentType type, const u64 startTimestamp, const u64 endTimestamp, const u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::DeleteAlbumFileByAruid(const ContentType type, const ApplicationFileEntry &entry, const u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileSizeByAruid(sf::Out<u64> out, const ApplicationFileEntry &entry, const u64 aruid) {
        return ResultSuccess();
    }

    //Result AlbumApplicationService::DeleteAlbumFileByAruidForDebug();

    Result AlbumApplicationService::LoadAlbumScreenShotImageByAruid(sf::Out<LoadAlbumScreenShotImageOutputForApplication> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const ApplicationFileEntry &entry, const CapsScreenShotDecodeOption &opts, const u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::LoadAlbumScreenShotThumbnailImageByAruid(sf::Out<LoadAlbumScreenShotImageOutputForApplication> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const ApplicationFileEntry &entry, const CapsScreenShotDecodeOption &opts, const u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::PrecheckToCreateContentsByAruid(const ContentType type, const u64 unk, const u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList1AafeAruidDeprecated(sf::OutBuffer buffer, sf::Out<u64> out, const ContentType type, const DateTime &start, const DateTime &end, const u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList2AafeUidAruidDeprecated(sf::OutBuffer buffer, sf::Out<u64> out, const ContentType type, const DateTime &start, const DateTime &end, AccountUid uid, const u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList3AaeAruid(sf::OutBuffer buffer, sf::Out<u64> out, const ContentType type, const DateTime &start, const DateTime &end, const u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList4AaeUidAruid(sf::OutBuffer buffer, sf::Out<u64> out, const ContentType type, const DateTime &start, const DateTime &end, AccountUid uid, const u64 aruid) {
        return ResultSuccess();
    }

    //Result AlbumApplicationService::OpenAccessorSessionForApplication();

}