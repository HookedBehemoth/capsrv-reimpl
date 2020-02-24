#pragma once
#include "../capsrv_types.hpp"

namespace ams::capsrv::ovl {

    void Initialize();
    void Exit();

    Result SetOverlayThumbnailData(const u8 *ptr, u64 size, const FileId &fileId, bool isMovie);
    Result GetLastOverlayThumbnail(u8 *ptr, u64 size, FileId *fileId, u64 *outSize, bool isMovie);

}