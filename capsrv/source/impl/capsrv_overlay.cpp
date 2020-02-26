#include "capsrv_overlay.hpp"

namespace ams::capsrv::ovl {

    constexpr u64 BufferSize = 0x5100;

    namespace {

        struct {
            u8 *buffer = nullptr;
            u64 size = 0;
            FileId fileId;
        } data[2];

        os::Mutex g_mutex;

    }

    void Initialize() {
        for (u8 i = 0; i < 2; i++) {
            data[i].buffer = (u8 *)malloc(BufferSize);
            std::memset(data[i].buffer, 0xff, BufferSize);
        }
    }

    void Exit() {
        for (u8 i = 0; i < 2; i++) {
            if (data[i].buffer)
                free(data[i].buffer);
            data[i].buffer = nullptr;
        }
    }

    Result SetOverlayThumbnailData(const u8 *ptr, u64 size, const FileId &fileId, bool isMovie) {
        R_UNLESS(size >= BufferSize, capsrv::ResultInvalidArgument());

        std::scoped_lock lk(g_mutex);
        data[(u8)isMovie].fileId = fileId;
        std::memcpy(data[(u8)isMovie].buffer, ptr, BufferSize);

        return ResultSuccess();
    }

    Result GetLastOverlayThumbnail(u8 *ptr, u64 size, FileId *fileId, u64 *outSize, bool isMovie) {
        R_UNLESS(size >= BufferSize, capsrv::ResultBufferInsufficient());
        
        std::scoped_lock lk(g_mutex);
        std::memcpy(ptr, data[(u8)isMovie].buffer, BufferSize);
        *outSize = 0x5100;
        *fileId = data[(u8)isMovie].fileId;

        return ResultSuccess();
    }

}