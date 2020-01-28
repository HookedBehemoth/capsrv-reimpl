#pragma once
#include "../capsrv_types.hpp"

namespace ams::capsrv::impl {

    Result InitializeAlbumAccessor();
    void FinalizeAlbumAccessor();

    Result GetAlbumFileCount(u64* outCount, const StorageId storageId);
    Result GetAlbumFileList(void* ptr, u64 size, u64* outCount, const StorageId storageId);
    Result LoadAlbumFile(void* ptr, u64 size, u64* outSize, const FileId &fileId);
    Result LoadAlbumScreenShotThumbnail(u64 *width, u64 *height, CapsScreenShotAttribute *attr, void* dat0, void* dat1, void* dat2, void* image, u64 imageSize, void* work, u64 workSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts);

}