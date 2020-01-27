#include "capsrv_album_accessor_service.hpp"

namespace ams::capsrv {

    Result AlbumAccessorService::GetAlbumFileCount(sf::Out<u64> out, const u8 storage) {
        out.SetValue(0);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumFileList(sf::OutBuffer buffer, sf::Out<u64> out, const u8 storage) {
        out.SetValue(0);
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumFile(sf::OutBuffer buffer, sf::Out<u64> out, const CapsAlbumFileId &fileId) {
        out.SetValue(0);
        return ResultSuccess();
    }

    Result AlbumAccessorService::DeleteAlbumFile(const CapsAlbumFileId &fileId) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::StorageCopyAlbumFile(const u8 storage, const CapsAlbumFileId &fileId) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::IsAlbumMounted(const u8 storage) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumUsage(sf::Out<CapsAlbumUsage2> out, const u8 storage) {
        out.SetValue({0});
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumFileSize(sf::Out<u64> out, const CapsAlbumFileId &fileId) {
        out.SetValue(0);
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumFileThumbnail(sf::OutBuffer image, sf::Out<u64> out, const CapsAlbumFileId &fileId) {
        out.SetValue(0);
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImage(sf::Out<u64> width, sf::Out<u64> height, sf::OutBuffer image, sf::OutNonSecureBuffer work, const CapsAlbumFileId &fileId) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImage(sf::Out<u64> width, sf::Out<u64> height, sf::OutBuffer image, sf::OutNonSecureBuffer work, const CapsAlbumFileId &fileId) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumEntryFromApplicationAlbumEntry(sf::Out<CapsAlbumEntry> out, const CapsApplicationAlbumEntry &appEntry, u64 appId) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImageEx(sf::Out<u64> width, sf::Out<u64> height, sf::OutBuffer image, sf::OutNonSecureBuffer work, const CapsAlbumFileId &fileId, const CapsScreenShotDecodeOption &opts) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImageEx(sf::Out<u64> width, sf::Out<u64> height, sf::OutBuffer image, sf::OutNonSecureBuffer work, const CapsAlbumFileId &fileId, const CapsScreenShotDecodeOption &opts) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImageEx0(sf::Out<CapsScreenShotAttribute> attrs, sf::Out<u64> width, sf::Out<u64> height, sf::OutBuffer image, sf::OutNonSecureBuffer work, const CapsAlbumFileId &fileId, const CapsScreenShotDecodeOption &opts) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumUsage3(sf::Out<CapsAlbumUsage3> usage, const u8 storage) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumMountResult(const u8 storage) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumUsage16(sf::OutFixedSizeBuffer usage, const u8 storage, const u8 flags) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetMinMaxAppletId(sf::OutBuffer minMax, sf::Out<bool> success) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumFileCountEx0(sf::Out<u64> out, const u8 storage, const u8 flags) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumFileListEx0(sf::OutFixedSizeBuffer buffer, sf::Out<u64> out, const u8 storage, const u8 flags) {
        return ResultSuccess();
    }

    //Result AlbumAccessorService::SaveEditedScreenShot

    Result AlbumAccessorService::GetLastOverlayScreenShotThumbnail(sf::OutBuffer buffer, sf::Out<CapsOverlayThumbnailData> out) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetLastOverlayMovieThumbnail(sf::OutBuffer buffer, sf::Out<CapsOverlayThumbnailData> out) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAutoSavingStorage(sf::Out<u8> out) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetRequiredStorageSpaceSizeToCopyAll(sf::Out<u64> out, const u8 dst, const u8 src) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImageEx0(sf::Out<CapsScreenShotAttribute> attrs, sf::Out<u64> width, sf::Out<u64> height, sf::OutBuffer image, sf::OutNonSecureBuffer work, const CapsAlbumFileId &fileId, const CapsScreenShotDecodeOption &opts) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImageEx1(sf::OutFixedSizeBuffer out, sf::OutBuffer image, sf::OutNonSecureBuffer work, const CapsAlbumFileId &fileId, const CapsScreenShotDecodeOption &opts) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImageEx1(sf::OutFixedSizeBuffer out, sf::OutBuffer image, sf::OutNonSecureBuffer work, const CapsAlbumFileId &fileId, const CapsScreenShotDecodeOption &opts) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::ForceAlbumUnmounted(const u8 storage) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::ResetAlbumMountStatus(const u8 storage) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::RefreshAlbumCache(const u8 storage) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumCache(sf::Out<CapsAlbumCache> cache, const u8 storage) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumCacheEx(sf::Out<CapsAlbumCache> cache, const u8 storage, const u8 type) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumEntryFromApplicationAlbumEntryAruid(sf::Out<CapsAlbumEntry> out, const CapsApplicationAlbumEntry &appEntry, u64 aruid) {
        return ResultSuccess();
    }

    Result AlbumAccessorService::SetInternalErrorConversionEnabled(const bool enabled) {
        return ResultSuccess();
    }

    //Result AlbumAccessorService::LoadMakerNoteInfoForDebug(sf::Out<u64> out, sf::OutBuffer unk0, sf::OutBuffer unk1, const CapsAlbumFileId &fileId)
    //Result AlbumAccessorService::OpenAccessorSession(sf::Out<std::shared_ptr<IContentStorage>> out, StorageId storage_id)
}