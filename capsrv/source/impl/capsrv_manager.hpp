#pragma once
#include "../capsrv_types.hpp"

namespace ams::capsrv::impl {

    Result GetAlbumFileCount(u64 *outCount, StorageId storage, u8 flags);
    Result GetAlbumFileList(void *ptr, u64 size, u64 *outCount, StorageId storage, u8 flags);

    Result DeleteAlbumFile(const FileId &fileId);
    Result CopyAlbumFile(StorageId storage, const FileId &fileId);
    Result GetAutoSavingStorage(StorageId *storage);

    Result IsAlbumMounted(bool *out, StorageId storage);
    Result MountAlbum(StorageId storage);
    Result UnmountAlbum(StorageId storage);

    Result GetAlbumCache(CapsAlbumCache *out, StorageId storage, ContentType type);

    Result SaveAlbumScreenShotFile(const u8 *buffer, u64 size, const FileId &fileId);

    /* Load Image. */
    Result LoadAlbumFile(void *ptr, u64 size, u64 *outSize, const FileId &fileId);
    Result LoadAlbumFileThumbnail(void *ptr, u64 size, u64 *out, const FileId &fileId);

    Result LoadAlbumScreenShotImage(u64 *width, u64 *height, void *work, u64 workSize, void *img, u64 imgSize, const FileId &fileId);
    Result LoadAlbumScreenShotThumbnailImage(u64 *width, u64 *height, void *work, u64 workSize, void *img, u64 imgSize, const FileId &fileId);

    Result LoadAlbumScreenShotImageEx(u64 *width, u64 *height, void *work, u64 workSize, void *img, u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts);
    Result LoadAlbumScreenShotThumbnailImageEx(u64 *width, u64 *height, void *work, u64 workSize, void *img, u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts);

    Result LoadAlbumScreenShotImageEx0(CapsScreenShotAttribute *attrs, u64 *width, u64 *height, void *work, u64 workSize, void *img, u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts);
    Result LoadAlbumScreenShotThumbnailImageEx0(CapsScreenShotAttribute *attrs, u64 *width, u64 *height, void *work, u64 workSize, void *img, u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts);

    Result LoadAlbumScreenShotImageEx1(LoadAlbumScreenShotImageOutput *out, void *work, u64 workSize, void *img, u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts);
    Result LoadAlbumScreenShotThumbnailImageEx1(LoadAlbumScreenShotImageOutput *out, void *work, u64 workSize, void *img, u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts);

    Result LoadAlbumScreenShotImageByAruid(LoadAlbumScreenShotImageOutputForApplication *out, void *work, u64 workSize, void *img, u64 imgSize, u64 aruid, const CapsApplicationAlbumFileEntry &appFileEntry, const CapsScreenShotDecodeOption &opts);
    Result LoadAlbumScreenShotThumbnailImageByAruid(LoadAlbumScreenShotImageOutputForApplication *out, void *work, u64 workSize, void *img, u64 imgSize, u64 aruid, const CapsApplicationAlbumFileEntry &appFileEntry, const CapsScreenShotDecodeOption &opts);

}