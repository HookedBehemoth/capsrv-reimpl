#pragma once
#include "../capsrv_types.hpp"
#include <switch.h>

namespace ams::capsrv::impl {

    Result InitializeAlbumAccessor();
    void FinalizeAlbumAccessor();

    Result GetAlbumFileCount(u64* outCount, const StorageId storageId);
    Result GetAlbumFileList(void* ptr, u64 size, u64* outCount, const StorageId storageId);
    Result LoadAlbumFile(void* ptr, u64 size, u64* outSize, const CapsAlbumFileId &fileId);

}