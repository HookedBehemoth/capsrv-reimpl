#pragma once
#include <stratosphere.hpp>

#include "../capsrv_types.hpp"
#include "capsrv_stream_session.hpp"

namespace ams::capsrv {

    /* Service definition. */
    class AlbumAccessorService final : public sf::IServiceObject {
      protected:
        /* Command IDs. */
        enum class CommandId {
            GetAlbumFileCount = 0,
            GetAlbumFileList = 1,
            LoadAlbumFile = 2,
            DeleteAlbumFile = 3,
            StorageCopyAlbumFile = 4,
            IsAlbumMounted = 5,
            GetAlbumUsage = 6,
            GetAlbumFileSize = 7,
            LoadAlbumFileThumbnail = 8,
            LoadAlbumScreenShotImage = 9,
            LoadAlbumScreenShotThumbnailImage = 10,
            GetAlbumEntryFromApplicationAlbumEntry = 11,
            LoadAlbumScreenShotImageEx = 12,
            LoadAlbumScreenShotThumbnailImageEx = 13,
            LoadAlbumScreenShotImageEx0 = 14,
            GetAlbumUsage3 = 15,
            GetAlbumMountResult = 16,
            GetAlbumUsage16 = 17,
            GetMinMaxAppletId = 18,
            GetAlbumFileCountEx0 = 100,
            GetAlbumFileListEx0 = 101,
            SaveEditedScreenShot = 202,
            GetLastOverlayScreenShotThumbnail = 301,
            GetLastOverlayMovieThumbnail = 302,
            GetAutoSavingStorage = 401,
            GetRequiredStorageSpaceSizeToCopyAll = 501,
            LoadAlbumScreenShotThumbnailImageEx0 = 1001,
            LoadAlbumScreenShotImageEx1 = 1002,
            LoadAlbumScreenShotThumbnailImageEx1 = 1003,
            ForceAlbumUnmounted = 8001,
            ResetAlbumMountStatus = 8002,
            RefreshAlbumCache = 8011,
            GetAlbumCache = 8012,
            GetAlbumCacheEx = 8013,
            GetAlbumEntryFromApplicationAlbumEntryAruid = 8021,
            SetInternalErrorConversionEnabled = 10011,
            LoadMakerNoteInfoForDebug = 50000,
            OpenAccessorSession = 60002,
        };

      public:
        virtual Result GetAlbumFileCount(sf::Out<u64> out, StorageId storage);
        virtual Result GetAlbumFileList(sf::OutBuffer buffer, sf::Out<u64> out, StorageId storage);
        virtual Result LoadAlbumFile(sf::OutBuffer image, sf::Out<u64> out, const FileId &fileId);
        virtual Result DeleteAlbumFile(const FileId &fileId);
        virtual Result StorageCopyAlbumFile(StorageId storage, const FileId &fileId);
        virtual Result IsAlbumMounted(sf::Out<bool> out, StorageId storage);
        virtual Result GetAlbumUsage(sf::Out<CapsAlbumUsage2> usage, StorageId storage);
        virtual Result GetAlbumFileSize(sf::Out<u64> out, const FileId &fileId);
        virtual Result LoadAlbumFileThumbnail(sf::OutBuffer image, sf::Out<u64> out, const FileId &fileId);
        virtual Result LoadAlbumScreenShotImage(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId);
        virtual Result LoadAlbumScreenShotThumbnailImage(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId);
        virtual Result GetAlbumEntryFromApplicationAlbumEntry(sf::Out<Entry> out, const ApplicationEntry &appEntry, u64 appId);
        virtual Result LoadAlbumScreenShotImageEx(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts);
        virtual Result LoadAlbumScreenShotThumbnailImageEx(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts);
        virtual Result LoadAlbumScreenShotImageEx0(sf::Out<CapsScreenShotAttribute> attrs, sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts);
        virtual Result GetAlbumUsage3(sf::Out<CapsAlbumUsage3> usage, StorageId storage);
        virtual Result GetAlbumMountResult(StorageId storage);
        virtual Result GetAlbumUsage16(sf::Out<AlbumUsage16> usage, StorageId storage, u8 flags);
        virtual Result GetMinMaxAppletId(sf::OutNonSecureBuffer minMax, sf::Out<bool> success);
        virtual Result GetAlbumFileCountEx0(sf::Out<u64> out, StorageId storage, u8 flags);
        virtual Result GetAlbumFileListEx0(sf::OutBuffer buffer, sf::Out<u64> out, u64 storage, u64 flags);
        virtual Result SaveEditedScreenShot(sf::InBuffer buf0, sf::InBuffer buf1, sf::Out<Entry> out, u64 a, u64 b, u64 c, u64 d, u64 e, u64 f, u64 g);
        virtual Result GetLastOverlayScreenShotThumbnail(sf::OutBuffer buffer, sf::Out<FileId> out, sf::Out<u64> size);
        virtual Result GetLastOverlayMovieThumbnail(sf::OutBuffer buffer, sf::Out<FileId> out, sf::Out<u64> size);
        virtual Result GetAutoSavingStorage(sf::Out<StorageId> out);
        virtual Result GetRequiredStorageSpaceSizeToCopyAll(sf::Out<u64> out, StorageId dst, StorageId src);
        virtual Result LoadAlbumScreenShotThumbnailImageEx0(sf::Out<CapsScreenShotAttribute> attrs, sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts);
        virtual Result LoadAlbumScreenShotImageEx1(sf::Out<LoadAlbumScreenShotImageOutput> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts);
        virtual Result LoadAlbumScreenShotThumbnailImageEx1(sf::Out<LoadAlbumScreenShotImageOutput> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts);
        virtual Result ForceAlbumUnmounted(StorageId storage);
        virtual Result ResetAlbumMountStatus(StorageId storage);
        virtual Result RefreshAlbumCache(StorageId storage);
        virtual Result GetAlbumCache(sf::Out<CapsAlbumCache> cache, StorageId storage);
        virtual Result GetAlbumCacheEx(sf::Out<CapsAlbumCache> cache, StorageId storage, const ContentType type);
        virtual Result GetAlbumEntryFromApplicationAlbumEntryAruid(sf::Out<Entry> out, const sf::ClientAppletResourceUserId &aruid, const ApplicationEntry &appEntry);
        virtual Result SetInternalErrorConversionEnabled(const bool enabled);
        virtual Result LoadMakerNoteInfoForDebug(sf::Out<u64> out, sf::OutBuffer unk0, sf::OutBuffer unk1, const FileId &fileId);
        virtual Result OpenAccessorSession(sf::Out<std::shared_ptr<AccessorSession>> out, const sf::ClientAppletResourceUserId &aruid);

      public:
        DEFINE_SERVICE_DISPATCH_TABLE{
            MAKE_SERVICE_COMMAND_META(GetAlbumFileCount),
            MAKE_SERVICE_COMMAND_META(GetAlbumFileList),
            MAKE_SERVICE_COMMAND_META(LoadAlbumFile),
            MAKE_SERVICE_COMMAND_META(DeleteAlbumFile),
            MAKE_SERVICE_COMMAND_META(StorageCopyAlbumFile),
            MAKE_SERVICE_COMMAND_META(IsAlbumMounted),
            MAKE_SERVICE_COMMAND_META(GetAlbumUsage),
            MAKE_SERVICE_COMMAND_META(GetAlbumFileSize),
            MAKE_SERVICE_COMMAND_META(LoadAlbumFileThumbnail),
            MAKE_SERVICE_COMMAND_META(LoadAlbumScreenShotImage, hos::Version_200),
            MAKE_SERVICE_COMMAND_META(LoadAlbumScreenShotThumbnailImage, hos::Version_200),
            MAKE_SERVICE_COMMAND_META(GetAlbumEntryFromApplicationAlbumEntry, hos::Version_200),
            MAKE_SERVICE_COMMAND_META(LoadAlbumScreenShotImageEx, hos::Version_300),
            MAKE_SERVICE_COMMAND_META(LoadAlbumScreenShotThumbnailImageEx, hos::Version_300),
            MAKE_SERVICE_COMMAND_META(LoadAlbumScreenShotImageEx0, hos::Version_300),
            MAKE_SERVICE_COMMAND_META(GetAlbumUsage3, hos::Version_400),
            MAKE_SERVICE_COMMAND_META(GetAlbumMountResult, hos::Version_400),
            MAKE_SERVICE_COMMAND_META(GetAlbumUsage16, hos::Version_400),
            MAKE_SERVICE_COMMAND_META(GetMinMaxAppletId, hos::Version_600),
            MAKE_SERVICE_COMMAND_META(GetAlbumFileCountEx0, hos::Version_500),
            MAKE_SERVICE_COMMAND_META(GetAlbumFileListEx0, hos::Version_500),
            MAKE_SERVICE_COMMAND_META(SaveEditedScreenShot, hos::Version_100, hos::Version_200),
            MAKE_SERVICE_COMMAND_META(GetLastOverlayScreenShotThumbnail),
            MAKE_SERVICE_COMMAND_META(GetLastOverlayMovieThumbnail, hos::Version_400),
            MAKE_SERVICE_COMMAND_META(GetAutoSavingStorage),
            MAKE_SERVICE_COMMAND_META(GetRequiredStorageSpaceSizeToCopyAll),
            MAKE_SERVICE_COMMAND_META(LoadAlbumScreenShotThumbnailImageEx0, hos::Version_300),
            MAKE_SERVICE_COMMAND_META(LoadAlbumScreenShotImageEx1, hos::Version_400),
            MAKE_SERVICE_COMMAND_META(LoadAlbumScreenShotThumbnailImageEx1, hos::Version_400),
            MAKE_SERVICE_COMMAND_META(ForceAlbumUnmounted),
            MAKE_SERVICE_COMMAND_META(ResetAlbumMountStatus),
            MAKE_SERVICE_COMMAND_META(RefreshAlbumCache),
            MAKE_SERVICE_COMMAND_META(GetAlbumCache),
            MAKE_SERVICE_COMMAND_META(GetAlbumCacheEx, hos::Version_400),
            MAKE_SERVICE_COMMAND_META(GetAlbumEntryFromApplicationAlbumEntryAruid, hos::Version_200),
            MAKE_SERVICE_COMMAND_META(SetInternalErrorConversionEnabled),
            MAKE_SERVICE_COMMAND_META(LoadMakerNoteInfoForDebug, hos::Version_600),
            MAKE_SERVICE_COMMAND_META(OpenAccessorSession, hos::Version_400)};
    };

}
