#pragma once
#include "../capsrv_types.hpp"

namespace ams::capsrv::impl {

Result GetAlbumFileCount(u64 *outCount, StorageId storage, u8 flags);
Result GetAlbumFileList(void *ptr, u64 size, u64 *outCount, StorageId storage, u8 flags);

Result DeleteAlbumFile(const FileId &fileId);

Result GetAutoSavingStorage(StorageId *storage);

Result IsAlbumMounted(bool *out, StorageId storage);
Result MountAlbum(StorageId storage);
Result UnmountAlbum(StorageId storage);

Result GetAlbumCache(CapsAlbumCache *out, StorageId storage, ContentType type);

} // namespace ams::capsrv::impl