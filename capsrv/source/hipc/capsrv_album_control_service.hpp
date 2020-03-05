#pragma once
#include <stratosphere.hpp>

#include "../capsrv_types.hpp"
#include "capsrv_stream_session.hpp"

namespace ams::capsrv {

    /* Service definition. */
    class AlbumControlService final : public sf::IServiceObject {
      protected:
        /* Command IDs. */
        enum class CommandId {
            CaptureRawImage = 1,
            CaptureRawImageWithTimeout = 2,
            SetShimLibraryVersion = 33,
            RequestTakingScreenShot = 1001,
            RequestTakingScreenShotWithTimeout = 1002,
            NotifyTakingScreenShotRefused = 1011,
            NotifyAlbumStorageIsAvailable = 2001,
            NotifyAlbumStorageIsUnavailable = 2002,
            RegisterAppletResourceUserId = 2011,
            UnregisterAppletResourceUserId = 2012,
            GetApplicationIdFromAruid = 2013,
            CheckApplicationIdRegistered = 2014,
            GenerateCurrentAlbumFileId = 2101,
            GenerateApplicationAlbumEntry = 2102,
            SaveAlbumScreenShotFile = 2201,
            SaveAlbumScreenShotFileEx = 2202,
            SetOverlayScreenShotThumbnailData = 2301,
            SetOverlayMovieThumbnailData = 2302,
            OpenControlSession = 60001,
        };

      public:
        virtual Result CaptureRawImage(sf::OutNonSecureBuffer out, u64 a, u64 b, u64 c, u64 d);
        virtual Result CaptureRawImageWithTimeout(sf::OutNonSecureBuffer out, u64 a, u64 b, u64 c, u64 d, u64 e);
        virtual Result SetShimLibraryVersion(u64 version, u64 aruid);
        virtual Result RequestTakingScreenShot(u64 a, u64 b);
        virtual Result RequestTakingScreenShotWithTimeout(u64 a, u64 b, u64 c);
        virtual Result NotifyTakingScreenShotRefused(u64 a);
        virtual Result NotifyAlbumStorageIsAvailable(StorageId storage);
        virtual Result NotifyAlbumStorageIsUnavailable(StorageId storage);
        virtual Result RegisterAppletResourceUserId(u64 aruid, u64 appId);
        virtual Result UnregisterAppletResourceUserId(u64 aruid, u64 appId);
        virtual Result GetApplicationIdFromAruid(sf::Out<u64> appId, u64 aruid);
        virtual Result CheckApplicationIdRegistered(u64 appId);
        virtual Result GenerateCurrentAlbumFileId(sf::Out<FileId> out, u64 appId, ContentType type);
        virtual Result GenerateApplicationAlbumEntry(sf::Out<ApplicationEntry> out, const Entry &entry, u64 appId);
        virtual Result SaveAlbumScreenShotFile(sf::InBuffer buffer, const FileId &fileId);
        virtual Result SaveAlbumScreenShotFileEx(sf::InNonSecureBuffer buffer, const FileId &fileId, u64 unk0, u64 unk1, u64 unk2);
        virtual Result SetOverlayScreenShotThumbnailData(sf::InNonSecureBuffer buffer, const FileId &fileId);
        virtual Result SetOverlayMovieThumbnailData(sf::InNonSecureBuffer buffer, const FileId &fileId);
        virtual Result OpenControlSession(sf::Out<std::shared_ptr<ControlSession>> out, const sf::ClientAppletResourceUserId &aruid);

      public:
        DEFINE_SERVICE_DISPATCH_TABLE{
            MAKE_SERVICE_COMMAND_META(CaptureRawImage, hos::Version_100, hos::Version_100),
            MAKE_SERVICE_COMMAND_META(CaptureRawImageWithTimeout, hos::Version_100, hos::Version_100),
            MAKE_SERVICE_COMMAND_META(SetShimLibraryVersion),
            MAKE_SERVICE_COMMAND_META(RequestTakingScreenShot, hos::Version_100, hos::Version_100),
            MAKE_SERVICE_COMMAND_META(RequestTakingScreenShotWithTimeout, hos::Version_100, hos::Version_100),
            MAKE_SERVICE_COMMAND_META(NotifyTakingScreenShotRefused, hos::Version_100, hos::Version_100),
            MAKE_SERVICE_COMMAND_META(NotifyAlbumStorageIsAvailable),
            MAKE_SERVICE_COMMAND_META(NotifyAlbumStorageIsUnavailable),
            MAKE_SERVICE_COMMAND_META(RegisterAppletResourceUserId),
            MAKE_SERVICE_COMMAND_META(UnregisterAppletResourceUserId),
            MAKE_SERVICE_COMMAND_META(GetApplicationIdFromAruid),
            MAKE_SERVICE_COMMAND_META(CheckApplicationIdRegistered),
            MAKE_SERVICE_COMMAND_META(GenerateCurrentAlbumFileId),
            MAKE_SERVICE_COMMAND_META(GenerateApplicationAlbumEntry),
            MAKE_SERVICE_COMMAND_META(SaveAlbumScreenShotFile),
            MAKE_SERVICE_COMMAND_META(SaveAlbumScreenShotFileEx),
            MAKE_SERVICE_COMMAND_META(SetOverlayScreenShotThumbnailData),
            MAKE_SERVICE_COMMAND_META(SetOverlayMovieThumbnailData),
            MAKE_SERVICE_COMMAND_META(OpenControlSession),
        };
    };
}
