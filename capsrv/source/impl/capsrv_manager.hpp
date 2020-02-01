#pragma once
#include <stratosphere.hpp>
#include "../capsrv_types.hpp"

namespace ams::capsrv::impl {

    Result InitializeAlbumAccessor();
    void FinalizeAlbumAccessor();

    StorageId GetPrimaryStorage();
    Result GetAutoSavingStorage(StorageId *storage);
    Result GenerateCurrentAlbumFileId(FileId *out, u64 appId, ContentType type);

    Result GetAlbumFileCount(u64* outCount, StorageId storageId);
    Result GetAlbumFileList(void* ptr, u64 size, u64* outCount, StorageId storageId);

    Result MountAlbum(StorageId storage);
    Result UnmountAlbum(StorageId storage);

    Result GetAlbumCache(CapsAlbumCache *out, StorageId storage, ContentType type);

}