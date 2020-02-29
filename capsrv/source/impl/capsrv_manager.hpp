#pragma once
#include "../capsrv_types.hpp"

namespace ams::capsrv::impl {

    /* Query operations. */
    Result GetAlbumFileCount(u64 *outCount, StorageId storage, u8 flags);
    Result GetAlbumFileList(void *ptr, u64 size, u64 *outCount, StorageId storage, u8 flags);
    Result RefreshAlbumCache(StorageId storage);
    Result GetRequiredStorageSpaceSizeToCopyAll(u64 *out, StorageId dst, StorageId src);
    Result GetAlbumUsage(CapsAlbumUsage2 *usage, StorageId storage);
    Result GetAlbumUsage3(CapsAlbumUsage3 *usage, StorageId storage);
    Result GetAlbumUsage16(CapsAlbumUsage16 *usage, StorageId storage, u8 flags);

    Result GetAlbumCache(CapsAlbumCache *out, StorageId storage, ContentType type);
    Result GetAutoSavingStorage(StorageId *storage);

    /* FS modifiers. */
    Result IsAlbumMounted(bool *out, StorageId storage);
    Result MountAlbum(StorageId storage);
    Result UnmountAlbum(StorageId storage);

    Result DeleteAlbumFile(const FileId &fileId);
    Result CopyAlbumFile(StorageId storage, const FileId &fileId);
    Result GetAlbumFileSize(u64 *out, const FileId &fileId);

    Result SaveAlbumScreenShotFile(const u8 *buffer, u64 size, const FileId &fileId);

    /* Load Image. */
    Result LoadAlbumFile(void *ptr, u64 size, u64 *outSize, const FileId &fileId);
    Result LoadAlbumFileThumbnail(void *ptr, u64 size, u64 *out, const FileId &fileId);

    Result LoadAlbumScreenShotImage(u64 *width, u64 *height, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId);
    Result LoadAlbumScreenShotThumbnailImage(u64 *width, u64 *height, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId);

    Result LoadAlbumScreenShotImageEx(u64 *width, u64 *height, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts);
    Result LoadAlbumScreenShotThumbnailImageEx(u64 *width, u64 *height, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts);

    Result LoadAlbumScreenShotImageEx0(CapsScreenShotAttribute *attrs, u64 *width, u64 *height, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts);
    Result LoadAlbumScreenShotThumbnailImageEx0(CapsScreenShotAttribute *attrs, u64 *width, u64 *height, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts);

    Result LoadAlbumScreenShotImageEx1(LoadAlbumScreenShotImageOutput *out, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts);
    Result LoadAlbumScreenShotThumbnailImageEx1(LoadAlbumScreenShotImageOutput *out, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts);

    Result LoadAlbumScreenShotImageByAruid(LoadAlbumScreenShotImageOutputForApplication *out, void *img, u64 img_size, void *work, u64 work_size, u64 aruid, const CapsApplicationAlbumFileEntry &appFileEntry, const CapsScreenShotDecodeOption &opts);
    Result LoadAlbumScreenShotThumbnailImageByAruid(LoadAlbumScreenShotImageOutputForApplication *out, void *img, u64 img_size, void *work, u64 work_size, u64 aruid, const CapsApplicationAlbumFileEntry &appFileEntry, const CapsScreenShotDecodeOption &opts);

}
