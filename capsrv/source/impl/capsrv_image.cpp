#include "capsrv_image.hpp"

namespace ams::capsrv::impl {

    namespace {

        Result LoadImage() { return ResultSuccess(); }
        Result LoadImageFromVideo() { return ResultSuccess(); }
        Result LoadFile(CapsScreenShotAttribute *attr, void *dst, u64 size, u64 *outSize, const FileId &fileId) {
            return fileId.type % 2 ? LoadImage() : LoadImageFromVideo();
        }

        Result LoadScreenShot(u64 *width, u64 *height, CapsScreenShotAttribute *attr, void *buf0, void *buf1, void *buf2, void *img, u64 imgSize, void *work, u64 workSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
            if (attr)
                *attr = {0};
            if (buf0)
                memset(buf0, 0, 0x400);
            if (buf1)
                memset(buf1, 0, 0x404);
            if (buf2)
                memset(buf2, 0, 0x400);
            CapsScreenShotAttribute tmpAttr = {0};
            u64 readSize;
            Result rc = LoadFile(nullptr, work, workSize, &readSize, fileId);
            if (rc.IsSuccess()) {
                rc = capsdcDecodeJpeg(1280, 720, &opts, work, workSize, img, imgSize);
                if (rc.IsSuccess()) {
                    *width = 1280;
                    *height = 720;
                    if (attr)
                        *attr = tmpAttr;
                    return ResultSuccess();
                }
                if (attr)
                    *attr = tmpAttr;
                if (buf0)
                    memset(buf0, 0, 0x400);
                if (buf1)
                    memset(buf1, 0, 0x404);
                if (buf2)
                    memset(buf2, 0, 0x400);
            }
            return rc;
        }

    } // namespace

    Result LoadAlbumFile(void *ptr, u64 size, u64 *outSize, const FileId &fileId) {
        return LoadFile(nullptr, ptr, size, outSize, fileId);
    }

    Result LoadAlbumFileThumbnail(void *ptr, u64 size, u64 *out, const FileId &fileId) { return ResultSuccess(); }

    Result LoadAlbumScreenShotImage(u64 *width, u64 *height, void *work, u64 workSize, void *img, u64 imgSize, const FileId &fileId) { return ResultSuccess(); }
    Result LoadAlbumScreenShotThumbnailImage(u64 *width, u64 *height, void *work, u64 workSize, void *img, u64 imgSize, const FileId &fileId) { return ResultSuccess(); }

    Result LoadAlbumScreenShotImageEx(u64 *width, u64 *height, void *work, u64 workSize, void *img, u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts) { return ResultSuccess(); }
    Result LoadAlbumScreenShotThumbnailImageEx(u64 *width, u64 *height, void *work, u64 workSize, void *img, u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts) { return ResultSuccess(); }

    Result LoadAlbumScreenShotImageEx0(CapsScreenShotAttribute *attrs, u64 *width, u64 *height, void *work, u64 workSize, void *img, u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts) { return ResultSuccess(); }
    Result LoadAlbumScreenShotThumbnailImageEx0(CapsScreenShotAttribute *attrs, u64 *width, u64 *height, void *work, u64 workSize, void *img, u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts) { return ResultSuccess(); }

    Result LoadAlbumScreenShotImageEx1(LoadAlbumScreenShotImageOutput *out, void *work, u64 workSize, void *img, u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts) { return ResultSuccess(); }
    Result LoadAlbumScreenShotThumbnailImageEx1(LoadAlbumScreenShotImageOutput *out, void *work, u64 workSize, void *img, u64 imgSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts) { return ResultSuccess(); }

}
