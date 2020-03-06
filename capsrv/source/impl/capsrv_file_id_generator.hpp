#pragma once
#include "../capsrv_types.hpp"

namespace ams::capsrv::impl {

    Result GenerateCurrentAlbumFileId(FileId *out, u64 appId, ContentType type);

}
