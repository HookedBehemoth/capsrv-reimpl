#pragma once
#include <stratosphere.hpp>

#include "../capsrv_types.hpp"

namespace ams::capsrv::impl {

Result InitializeAlbumAccessor();
void FinalizeAlbumAccessor();

Result GenerateCurrentAlbumFileId(FileId *out, u64 appId, ContentType type);

} // namespace ams::capsrv::impl