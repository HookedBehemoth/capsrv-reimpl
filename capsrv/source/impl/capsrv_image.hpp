#pragma once
#include "../capsrv_types.hpp"

#include <vapours/results.hpp>

namespace ams::capsrv::impl {

    Result LoadAlbumFile(void* ptr, u64 size, u64* outSize, const FileId &fileId);
    Result LoadAlbumFileThumbnail(void* ptr, u64 size, u64 *out, const FileId &fileId);
    
    Result LoadAlbumScreenShotImage(u64 *width, u64 *height, void* work, const u64 workSize, void* img, const u64 imgSize, const FileId &fileId);
    Result LoadAlbumScreenShotThumbnailImage(u64 *width, u64 *height, void* work, const u64 workSize, void* img, const u64 imgSize, const FileId &fileId);

    Result LoadAlbumScreenShotImageEx(u64 *width, u64 *height, void* work, const u64 workSize, void* img, const u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts);
    Result LoadAlbumScreenShotThumbnailImageEx(u64 *width, u64 *height, void* work, const u64 workSize, void* img, const u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts);

    Result LoadAlbumScreenShotImageEx0(CapsScreenShotAttribute *attrs, u64 *width, u64 *height, void* work, const u64 workSize, void* img, const u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts);
    Result LoadAlbumScreenShotThumbnailImageEx0(CapsScreenShotAttribute *attrs, u64 *width, u64 *height, void* work, const u64 workSize, void* img, const u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts);

    Result LoadAlbumScreenShotImageEx1(LoadAlbumScreenShotImageOutput *out, void* work, const u64 workSize, void* img, const u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts);
    Result LoadAlbumScreenShotThumbnailImageEx1(LoadAlbumScreenShotImageOutput *out, void* work, const u64 workSize, void* img, const u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts);

}
