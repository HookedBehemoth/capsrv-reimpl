#include "video_extractor.hpp"

constexpr u64 MAX_JPEG_SIZE = 0x80000;
constexpr u64 BLOCK_COUNT(u64 size) { return size >> 0x12; };
constexpr u64 BLOCK_SIZE = 0x20;
constexpr u64 HASH_SIZE = 0x28;

constexpr u64 MIN_SIZE() {
    return MAX_JPEG_SIZE + (BLOCK_COUNT(MAX_JPEG_SIZE) * BLOCK_SIZE) + HASH_SIZE;
}

namespace ams::video {

    Result GetOffsets(u64 *jpegOffset, u64 *jpegSize, u64 *macOffset, u64 *macSize, u64 mpegSize, u64 mpegTail) {
        R_UNLESS(mpegTail == 1,             capsrv::ResultAlbumInvalidFileData());
        R_UNLESS(mpegSize >= MIN_SIZE(),    capsrv::ResultAlbumInvalidFileData());

        u64 blockCount = BLOCK_COUNT(mpegSize);
        *jpegOffset = (mpegSize - MAX_JPEG_SIZE) & 0xfffffffffffc0000;
        *jpegSize = MAX_JPEG_SIZE;
        *macOffset = mpegSize & 0xfffffffffffc0000;
        *macSize = blockCount << 0x5;
        return 0;
    }

}