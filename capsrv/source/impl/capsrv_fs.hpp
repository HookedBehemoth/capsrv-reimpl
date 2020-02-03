#pragma once
#include "../capsrv_types.hpp"

namespace ams::capsrv::impl {

    Result DeleteAlbumFile(const FileId &fileId);

    Result GetAutoSavingStorage(StorageId *storage);

    Result IsAlbumMounted(bool *out, StorageId storage);
    Result MountAlbum(StorageId storage);
    Result UnmountAlbum(StorageId storage);

    Result GetAlbumCache(CapsAlbumCache *out, StorageId storage, ContentType type);

}