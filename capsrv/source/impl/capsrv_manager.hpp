#pragma once
#include <stratosphere.hpp>
#include "../capsrv_types.hpp"

namespace ams::capsrv::impl {

    Result InitializeAlbumAccessor();
    void FinalizeAlbumAccessor();

    Result GenerateCurrentAlbumFileId(FileId *out, u64 appId, ContentType type);

    Result GetAlbumFileCount(u64* outCount, StorageId storageId);
    Result GetAlbumFileList(void* ptr, u64 size, u64* outCount, StorageId storageId);

}