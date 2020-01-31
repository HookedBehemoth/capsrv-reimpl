#pragma once
#include <stratosphere.hpp>
#include "../capsrv_types.hpp"

namespace ams::capsrv::impl {

    Result InitializeAlbumAccessor();
    void FinalizeAlbumAccessor();

    StorageId GetPrimaryStorage();
    Result GetAutoSavingStorage(StorageId *storage);
    Result GenerateCurrentAlbumFileId(FileId *out, u64 appId, ContentType type);

    Result GetAlbumFileCount(u64* outCount, const StorageId storageId);
    Result GetAlbumFileList(void* ptr, u64 size, u64* outCount, const StorageId storageId);

    Result MountAlbum(const StorageId storage);
    Result UnmountAlbum(const StorageId storage);

    Result GetAlbumCache(CapsAlbumCache *out, const StorageId storage, const ContentType type);

}