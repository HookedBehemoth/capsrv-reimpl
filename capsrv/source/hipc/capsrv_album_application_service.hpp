#pragma once
#include <stratosphere.hpp>

#include "../capsrv_types.hpp"
#include "capsrv_stream_session.hpp"

namespace ams::capsrv {

    /* Service definition. */
    class AlbumApplicationService final : public sf::IServiceObject {
      protected:
        /* Command IDs. */
        enum class CommandId {
            SetShimLibraryVersion = 32,
            GetAlbumFileList0AafeAruidDeprecated = 102,
            DeleteAlbumFileByAruid = 103,
            GetAlbumFileSizeByAruid = 104,
            DeleteAlbumFileByAruidForDebug = 105,
            LoadAlbumScreenShotImageByAruid = 110,
            LoadAlbumScreenShotThumbnailImageByAruid = 120,
            PrecheckToCreateContentsByAruid = 130,
            GetAlbumFileList1AafeAruidDeprecated = 140,
            GetAlbumFileList2AafeUidAruidDeprecated = 141,
            GetAlbumFileList3AaeAruid = 142,
            GetAlbumFileList4AaeUidAruid = 143,
            OpenAccessorSessionForApplication = 60002
        };

      public:
        virtual Result SetShimLibraryVersion(u64 version, const sf::ClientAppletResourceUserId &aruid);
        virtual Result GetAlbumFileList0AafeAruidDeprecated(sf::OutBuffer buffer, const sf::ClientAppletResourceUserId &aruid, ContentType type, u64 startTimestamp, u64 endTimestamp);
        virtual Result DeleteAlbumFileByAruid(const sf::ClientAppletResourceUserId &aruid, ContentType type, const CapsApplicationAlbumFileEntry &entry);
        virtual Result GetAlbumFileSizeByAruid(sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, const CapsApplicationAlbumFileEntry &entry);
        virtual Result DeleteAlbumFileByAruidForDebug(const sf::ClientAppletResourceUserId &aruid, const CapsApplicationAlbumFileEntry &entry);
        virtual Result LoadAlbumScreenShotImageByAruid(sf::Out<LoadAlbumScreenShotImageOutputForApplication> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const sf::ClientAppletResourceUserId &aruid, const CapsApplicationAlbumFileEntry &entry, const CapsScreenShotDecodeOption &opts);
        virtual Result LoadAlbumScreenShotThumbnailImageByAruid(sf::Out<LoadAlbumScreenShotImageOutputForApplication> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const sf::ClientAppletResourceUserId &aruid, const CapsApplicationAlbumFileEntry &entry, const CapsScreenShotDecodeOption &opts);
        virtual Result PrecheckToCreateContentsByAruid(ContentType type, u64 unk, const sf::ClientAppletResourceUserId &aruid);
        virtual Result GetAlbumFileList1AafeAruidDeprecated(sf::OutBuffer buffer, sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, ContentType type, const DateTime &start, const DateTime &end);
        virtual Result GetAlbumFileList2AafeUidAruidDeprecated(sf::OutBuffer buffer, sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, ContentType type, const DateTime &start, const DateTime &end, AccountUid uid);
        virtual Result GetAlbumFileList3AaeAruid(sf::OutBuffer buffer, sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, ContentType type, const DateTime &start, const DateTime &end);
        virtual Result GetAlbumFileList4AaeUidAruid(sf::OutBuffer buffer, sf::Out<u64> out, const sf::ClientAppletResourceUserId &aruid, ContentType type, const DateTime &start, const DateTime &end, AccountUid uid);
        virtual Result OpenAccessorSessionForApplication(sf::Out<std::shared_ptr<AccessorApplicationSession>> out, const sf::ClientAppletResourceUserId &aruid, const CapsApplicationAlbumFileEntry &entry);

      public:
        DEFINE_SERVICE_DISPATCH_TABLE{
            MAKE_SERVICE_COMMAND_META(SetShimLibraryVersion),
            MAKE_SERVICE_COMMAND_META(GetAlbumFileList0AafeAruidDeprecated),
            MAKE_SERVICE_COMMAND_META(DeleteAlbumFileByAruid),
            MAKE_SERVICE_COMMAND_META(GetAlbumFileSizeByAruid),
            MAKE_SERVICE_COMMAND_META(DeleteAlbumFileByAruidForDebug),
            MAKE_SERVICE_COMMAND_META(LoadAlbumScreenShotImageByAruid),
            MAKE_SERVICE_COMMAND_META(LoadAlbumScreenShotThumbnailImageByAruid),
            MAKE_SERVICE_COMMAND_META(PrecheckToCreateContentsByAruid),
            MAKE_SERVICE_COMMAND_META(GetAlbumFileList1AafeAruidDeprecated, hos::Version_600),
            MAKE_SERVICE_COMMAND_META(GetAlbumFileList2AafeUidAruidDeprecated, hos::Version_600),
            MAKE_SERVICE_COMMAND_META(GetAlbumFileList3AaeAruid, hos::Version_700),
            MAKE_SERVICE_COMMAND_META(GetAlbumFileList4AaeUidAruid, hos::Version_700),
            MAKE_SERVICE_COMMAND_META(OpenAccessorSessionForApplication),
        };
    };

}
