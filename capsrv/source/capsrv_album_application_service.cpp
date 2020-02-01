#include "capsrv_album_application_service.hpp"

namespace ams::capsrv {

    Result AlbumApplicationService::SetShimLibraryVersion( u64 version,  u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList0AafeAruidDeprecated(sf::OutBuffer buffer, ContentType type,  u64 startTimestamp,  u64 endTimestamp,  u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::DeleteAlbumFileByAruid(ContentType type, const ApplicationFileEntry &entry,  u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileSizeByAruid(sf::Out<u64> out, const ApplicationFileEntry &entry,  u64 aruid) {
        return ResultSuccess();
    }

    //Result AlbumApplicationService::DeleteAlbumFileByAruidForDebug();

    Result AlbumApplicationService::LoadAlbumScreenShotImageByAruid(sf::Out<LoadAlbumScreenShotImageOutputForApplication> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const ApplicationFileEntry &entry, const CapsScreenShotDecodeOption &opts,  u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::LoadAlbumScreenShotThumbnailImageByAruid(sf::Out<LoadAlbumScreenShotImageOutputForApplication> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const ApplicationFileEntry &entry, const CapsScreenShotDecodeOption &opts,  u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::PrecheckToCreateContentsByAruid(ContentType type,  u64 unk,  u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList1AafeAruidDeprecated(sf::OutBuffer buffer, sf::Out<u64> out, ContentType type, const DateTime &start, const DateTime &end,  u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList2AafeUidAruidDeprecated(sf::OutBuffer buffer, sf::Out<u64> out, ContentType type, const DateTime &start, const DateTime &end, AccountUid uid,  u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList3AaeAruid(sf::OutBuffer buffer, sf::Out<u64> out, ContentType type, const DateTime &start, const DateTime &end,  u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumApplicationService::GetAlbumFileList4AaeUidAruid(sf::OutBuffer buffer, sf::Out<u64> out, ContentType type, const DateTime &start, const DateTime &end, AccountUid uid,  u64 aruid) {
        return ResultSuccess();
    }

    //Result AlbumApplicationService::OpenAccessorSessionForApplication();

}