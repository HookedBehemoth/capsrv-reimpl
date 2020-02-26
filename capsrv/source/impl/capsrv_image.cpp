#include "capsrv_image.hpp"

#include "capsrv_controller.hpp"

namespace ams::capsrv::impl {

    namespace {

        Result LoadImage(CapsScreenShotAttribute *out_attr, void *buf_0, void *buf_1, void *buf_2, u64 *out_size, void *work, u64 work_size, const FileId &fileId) {
            if (out_attr)
                *out_attr = {0};
            if (buf_0)
                memset(buf_0, 0, 0x400);
            if (buf_1)
                memset(buf_1, 0, 0x404);
            if (buf_2)
                memset(buf_2, 0, 0x400);

            std::memset(work, 0, work_size);

            /* TODO: Is Reserved */
            /* TODO: Is Valid */
            /* TODO: Mount Album */
            /* TODO: Is supported */
            /* TODO: Load thumbnail. */

            if (out_attr)
                *out_attr = {0};
            if (buf_0)
                memset(buf_0, 0, 0x400);
            if (buf_1)
                memset(buf_1, 0, 0x404);
            if (buf_2)
                memset(buf_2, 0, 0x400);
            return ResultSuccess();
        }

        Result LoadScreenShotImage(Dimensions *dims, CapsScreenShotAttribute *out_attr, void *buf_0, void *buf_1, void *buf_2, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts = {0}) {
            CapsScreenShotAttribute attr{};
            u64 jpegSize = 0;
            Result rc = LoadImage(out_attr, buf_0, buf_1, buf_2, &jpegSize, work, work_size, fileId);
            if (R_SUCCEEDED(rc)) {
                rc = capsdcDecodeJpeg(1280, 720, &opts, work, jpegSize, img, img_size);
                if (R_SUCCEEDED(rc)) {
                    dims->width = 1280;
                    dims->height = 720;
                    if (out_attr) {
                        *out_attr = attr;
                    }
                    return rc;
                }
            }
            if (out_attr)
                *out_attr = {0};
            if (buf_0)
                memset(buf_0, 0, 0x400);
            if (buf_1)
                memset(buf_1, 0, 0x404);
            if (buf_2)
                memset(buf_2, 0, 0x400);

            return rc;
        }

        Result LoadThumbnail(CapsScreenShotAttribute *out_attr, void *buf_0, void *buf_1, void *buf_2, u64 *out_size, void *work, u64 work_size, const FileId &fileId) {
            if (out_attr)
                *out_attr = {0};
            if (buf_0)
                memset(buf_0, 0, 0x400);
            if (buf_1)
                memset(buf_1, 0, 0x404);
            if (buf_2)
                memset(buf_2, 0, 0x400);

            std::memset(work, 0, work_size);

            /* TODO: Is Reserved */
            /* TODO: Is Valid */
            /* TODO: Mount Album */
            /* TODO: Is supported */
            /* TODO: Load thumbnail. */

            if (out_attr)
                *out_attr = {0};
            if (buf_0)
                memset(buf_0, 0, 0x400);
            if (buf_1)
                memset(buf_1, 0, 0x404);
            if (buf_2)
                memset(buf_2, 0, 0x400);
            return ResultSuccess();
        }

        Result LoadScreenShotThumbnail(Dimensions *dims, CapsScreenShotAttribute *out_attr, void *buf_0, void *buf_1, void *buf_2, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts = {0}) {
            CapsScreenShotAttribute attr{};
            u64 jpegSize = 0;
            Result rc = LoadThumbnail(out_attr, buf_0, buf_1, buf_2, &jpegSize, work, work_size, fileId);
            if (R_SUCCEEDED(rc)) {
                rc = capsdcDecodeJpeg(320, 180, &opts, work, work_size, img, img_size);
                if (R_SUCCEEDED(rc)) {
                    dims->width = 1280;
                    dims->height = 720;
                    if (out_attr) {
                        *out_attr = attr;
                    }
                    return rc;
                }
            }
            if (out_attr)
                *out_attr = {0};
            if (buf_0)
                memset(buf_0, 0, 0x400);
            if (buf_1)
                memset(buf_1, 0, 0x404);
            if (buf_2)
                memset(buf_2, 0, 0x400);

            return rc;
        }

    }

    Result LoadAlbumFile(void *ptr, u64 size, u64 *outSize, const FileId &fileId) {
        return LoadImage(nullptr, nullptr, nullptr, nullptr, outSize, ptr, size, fileId);
    }

    Result LoadAlbumFileThumbnail(void *ptr, u64 size, u64 *outSize, const FileId &fileId) {
        return LoadThumbnail(nullptr, nullptr, nullptr, nullptr, outSize, ptr, size, fileId);
    }

    Result LoadAlbumScreenShotImage(u64 *width, u64 *height, void *work, u64 work_size, void *img, u64 img_size, const FileId &fileId) {
        Result rc{};
        Dimensions dims{};
        {
            /* Lock Mutex */
            rc = LoadScreenShotImage(&dims, nullptr, nullptr, nullptr, nullptr, img, img_size, work, work_size, fileId);
        }
        if (R_SUCCEEDED(rc)) {
            if (width)
                *width = dims.width;
            if (height)
                *height = dims.height;
        }
        std::memset(work, 0, work_size);
        return rc;
    }

    Result LoadAlbumScreenShotThumbnailImage(u64 *width, u64 *height, void *work, u64 work_size, void *img, u64 img_size, const FileId &fileId) {
        Result rc{};
        Dimensions dims{};
        {
            /* Lock Mutex */
            rc = LoadScreenShotThumbnail(&dims, nullptr, nullptr, nullptr, nullptr, img, img_size, work, work_size, fileId);
        }
        if (R_SUCCEEDED(rc)) {
            if (width)
                *width = dims.width;
            if (height)
                *height = dims.height;
        }
        std::memset(work, 0, work_size);
        return rc;
    }

    Result LoadAlbumScreenShotImageEx(u64 *width, u64 *height, void *work, u64 work_size, void *img, u64 img_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        {
            /* Lock Mutex */
            rc = LoadScreenShotImage(&dims, nullptr, nullptr, nullptr, nullptr, img, img_size, work, work_size, fileId, opts);
        }
        if (R_SUCCEEDED(rc)) {
            if (width)
                *width = dims.width;
            if (height)
                *height = dims.height;
        }
        std::memset(work, 0, work_size);
        return rc;
    }
    Result LoadAlbumScreenShotThumbnailImageEx(u64 *width, u64 *height, void *work, u64 work_size, void *img, u64 img_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        {
            /* Lock Mutex */
            rc = LoadScreenShotThumbnail(&dims, nullptr, nullptr, nullptr, nullptr, img, img_size, work, work_size, fileId, opts);
        }
        if (R_SUCCEEDED(rc)) {
            if (width)
                *width = dims.width;
            if (height)
                *height = dims.height;
        }
        std::memset(work, 0, work_size);
        return rc;
    }

    Result LoadAlbumScreenShotImageEx0(CapsScreenShotAttribute *out_attr, u64 *width, u64 *height, void *work, u64 work_size, void *img, u64 img_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        CapsScreenShotAttribute attr{};
        {
            /* Lock Mutex */
            rc = LoadScreenShotImage(&dims, &attr, nullptr, nullptr, nullptr, img, img_size, work, work_size, fileId, opts);
        }
        if (R_SUCCEEDED(rc)) {
            if (width)
                *width = dims.width;
            if (height)
                *height = dims.height;
            if (out_attr)
                *out_attr = attr;
        }
        std::memset(work, 0, work_size);
        return rc;
    }
    Result LoadAlbumScreenShotThumbnailImageEx0(CapsScreenShotAttribute *out_attr, u64 *width, u64 *height, void *work, u64 work_size, void *img, u64 img_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        CapsScreenShotAttribute attr{};
        {
            /* Lock Mutex */
            rc = LoadScreenShotThumbnail(&dims, &attr, nullptr, nullptr, nullptr, img, img_size, work, work_size, fileId, opts);
        }
        if (R_SUCCEEDED(rc)) {
            if (width)
                *width = dims.width;
            if (height)
                *height = dims.height;
            if (out_attr)
                *out_attr = attr;
        }
        std::memset(work, 0, work_size);
        return rc;
    }

    Result LoadAlbumScreenShotImageEx1(LoadAlbumScreenShotImageOutput *out, void *work, u64 work_size, void *img, u64 img_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        CapsScreenShotAttribute attr{};
        {
            /* Lock Mutex */
            rc = LoadScreenShotImage(&dims, &attr, out->unk_x50, nullptr, nullptr, img, img_size, work, work_size, fileId, opts);
        }
        if (R_SUCCEEDED(rc) && out) {
            out->width = dims.width;
            out->height = dims.height;
            out->attr = attr;
        }
        std::memset(work, 0, work_size);
        return rc;
    }
    Result LoadAlbumScreenShotThumbnailImageEx1(LoadAlbumScreenShotImageOutput *out, void *work, u64 work_size, void *img, u64 img_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        CapsScreenShotAttribute attr{};
        {
            /* Lock Mutex */
            rc = LoadScreenShotThumbnail(&dims, &attr, out->unk_x50, nullptr, nullptr, img, img_size, work, work_size, fileId, opts);
        }
        if (R_SUCCEEDED(rc) && out) {
            out->width = dims.width;
            out->height = dims.height;
            out->attr = attr;
        }
        std::memset(work, 0, work_size);
        return rc;
    }

    Result LoadAlbumScreenShotImageByAruid(LoadAlbumScreenShotImageOutputForApplication *out, void *work, u64 work_size, void *img, u64 img_size, u64 aruid, const CapsApplicationAlbumFileEntry &appFileEntry, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        CapsScreenShotAttribute attr{};
        Entry entry{};
        control::GetAlbumEntryFromApplicationAlbumEntryAruid(&entry, (ApplicationEntry *)&appFileEntry.entry, aruid);
        {
            /* Lock Mutex */
            rc = LoadScreenShotImage(&dims, &attr, nullptr, &out->appdata, nullptr, img, img_size, work, work_size, entry.fileId, opts);
        }
        if (R_SUCCEEDED(rc) && out) {
            out->width = dims.width;
            out->height = dims.height;
            (out->attr).unk_x0 = attr.orientation;
            (out->attr).unk_x4 = attr.unk_x0;
            (out->attr).unk_x5 = attr.reserved[2];
            (out->attr).unk_x6 = attr.reserved[1];
            (out->attr).pad = '\0';
            (out->attr).unk_x8 = attr.reserved[0];
            (out->attr).unk_xc = attr.unk_xc;
            (out->attr).unk_x10 = attr.unk_x10;
            (out->attr).unk_x14 = attr.unk_x14;
            (out->attr).unk_x18 = attr.unk_x18;
            (out->attr).unk_x1c = attr.unk_x1c;
            (out->attr).unk_x20 = attr.unk_x20;
            (out->attr).unk_x22 = attr.unk_x22;
            (out->attr).unk_x24 = attr.unk_x24;
            (out->attr).unk_x26 = attr.unk_x26;
            (out->attr).reserved[1] = 0;
            (out->attr).reserved[2] = 0;
            (out->attr).reserved[0] = 0;
        }
        return rc;
    }
    Result LoadAlbumScreenShotThumbnailImageByAruid(LoadAlbumScreenShotImageOutputForApplication *out, void *work, u64 work_size, void *img, u64 img_size, u64 aruid, const CapsApplicationAlbumFileEntry &appFileEntry, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        CapsScreenShotAttribute attr{};
        Entry entry{};
        control::GetAlbumEntryFromApplicationAlbumEntryAruid(&entry, (ApplicationEntry *)&appFileEntry.entry, aruid);
        {
            /* Lock Mutex */
            rc = LoadScreenShotThumbnail(&dims, &attr, nullptr, &out->appdata, nullptr, img, img_size, work, work_size, entry.fileId, opts);
        }
        if (R_SUCCEEDED(rc) && out) {
            out->width = dims.width;
            out->height = dims.height;
            (out->attr).unk_x0 = attr.orientation;
            (out->attr).unk_x4 = attr.unk_x0;
            (out->attr).unk_x5 = attr.reserved[2];
            (out->attr).unk_x6 = attr.reserved[1];
            (out->attr).pad = '\0';
            (out->attr).unk_x8 = attr.reserved[0];
            (out->attr).unk_xc = attr.unk_xc;
            (out->attr).unk_x10 = attr.unk_x10;
            (out->attr).unk_x14 = attr.unk_x14;
            (out->attr).unk_x18 = attr.unk_x18;
            (out->attr).unk_x1c = attr.unk_x1c;
            (out->attr).unk_x20 = attr.unk_x20;
            (out->attr).unk_x22 = attr.unk_x22;
            (out->attr).unk_x24 = attr.unk_x24;
            (out->attr).unk_x26 = attr.unk_x26;
            (out->attr).reserved[1] = 0;
            (out->attr).reserved[2] = 0;
            (out->attr).reserved[0] = 0;
        }
        return rc;
    }

}
