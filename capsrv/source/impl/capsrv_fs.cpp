#include "capsrv_fs.hpp"

#include <machine/endian.h>

#include <filesystem>
#include <map>
#include <string>

#include "../capsrv_config.hpp"
#include "../capsrv_util.hpp"

namespace ams::capsrv::impl {

    constexpr const char *mountNames[] = {
        [StorageId::Nand] = "NA",
        [StorageId::Sd] = "SD",
    };

    constexpr const char *mountPoints[] = {
        [StorageId::Nand] = "NA:/",
        [StorageId::Sd] = "SD:/",
    };

    constexpr const char *fileExtensions[] = {
        [0] = ".jpg",
        [1] = ".mp4",
    };

    namespace {

        ContentStorage g_storage;
        bool g_mountStatus[2] = {false, false};
        struct ReserveItem {
            bool used;
            FileId fileId;
        };
        struct Reserve {
            ReserveItem items[0xa];
        } readReserve, writeReserve;

        bool IsReserved(const FileId &fileId, const Reserve &reserve) {
            for (const auto &item : reserve.items) {
                if (item.used && std::memcmp(&item.fileId, &fileId, 0x18) == 0)
                    return true;
            }
            return false;
        }

        StorageId GetPrimaryStorage() {
            SetSysPrimaryAlbumStorage storage;
            Result rc = setsysGetPrimaryAlbumStorage(&storage);

            if (R_FAILED(rc))
                std::abort();

            return StorageId(storage);
        }

        Result MountImageDirectory(StorageId storage) {
            FsFileSystem imgDirFs;
            R_TRY(fsOpenImageDirectoryFileSystem(&imgDirFs, FsImageDirectoryId(storage)));

            if (fsdevMountDevice(mountNames[storage], imgDirFs) == -1) {
                fsFsClose(&imgDirFs);
                std::abort();
            }

            return ResultSuccess();
        }

        Result MountCustomImageDirectory(const char *path) {
            if (nullptr)
                return fs::ResultNullptrArgument();

            // TODO: Mount Host PC when TMA is a thing.
            return fs::ResultHostFileSystemCorrupted();
        }

        Result UnmountImageDirectory(StorageId storage) {
            // TODO: Unmount Host PC when TMA is a thing.
            return fsdevUnmountDevice(mountNames[storage]);
        }

        const char *GetMountName(StorageId storage) {
            return mountNames[storage];
        }

        struct ProcessObject {};

        struct CountObject : ProcessObject {
            u64 count;
            u8 flags;
        };

        bool cb_count(const Entry &entry, ProcessObject *ptr) {
            CountObject *user = (CountObject *)ptr;
            if ((user->flags | BIT(0) && entry.fileId.type % 2 == ContentType::Screenshot) ||
                (user->flags | BIT(1) && entry.fileId.type % 2 == ContentType::Movie))
                user->count++;
            return true;
        }

        struct ListObject : ProcessObject {
            Entry *entries;
            u64 size;
            u64 count;
            u8 flags;
        };

        bool cb_list(const Entry &entry, ProcessObject *ptr) {
            ListObject *user = (ListObject *)ptr;
            if (user->size == user->count)
                return false;

            if ((user->flags | BIT(0) && entry.fileId.type % 2 == ContentType::Screenshot) ||
                (user->flags | BIT(1) && entry.fileId.type % 2 == ContentType::Movie)) {
                *user->entries = entry;
                user->entries++;
                user->count++;
            }
            return true;
        }

        Result ProcessImageDirectory(StorageId storage, std::function<bool(const Entry &, ProcessObject *)> callback, ProcessObject *user) {
            for (auto &e : std::filesystem::recursive_directory_iterator(mountPoints[storage])) {
                if (e.is_regular_file()) {
                    FileId fileId = {0};
                    Result rc = FileId::FromString(&fileId, storage, e.path().filename().c_str());
                    Entry entry = {
                        .size = e.file_size(),
                        .fileId = fileId,
                    };
                    if (rc.IsSuccess())
                        if (!callback(entry, user))
                            break;
                }
            }
            return ResultSuccess();
        }

    }

    Result GetAlbumFileCount(u64 *outCount, StorageId storage, u8 flags) {
        R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
        CountObject countObject = {
            .count = 0,
            .flags = flags};
        Result res = 0;
        {
            res = ProcessImageDirectory(storage, cb_count, &countObject);
        }
        if (res.IsSuccess() && outCount)
            *outCount = countObject.count;
        return res;
    }

    Result GetAlbumFileList(void *ptr, u64 size, u64 *outCount, StorageId storage, u8 flags) {
        R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
        ListObject listObject = {
            .entries = (Entry *)ptr,
            .size = size,
            .count = 0,
            .flags = flags};
        Result res = 0;
        {
            res = ProcessImageDirectory(storage, cb_list, &listObject);
        }
        if (res.IsSuccess() && outCount)
            *outCount = listObject.count;
        return res;
    }

    Result DeleteAlbumFile(const FileId &fileId) {
        R_UNLESS(!IsReserved(fileId, readReserve), 0xbcce);
        R_UNLESS(!IsReserved(fileId, writeReserve), 0xbcce);
        R_TRY(fileId.Verify());
        R_TRY(MountAlbum(fileId.storage));
        std::filesystem::remove(fileId.GetFilePath());
        g_storage.Decrement(fileId.storage, fileId.type);
        return ResultSuccess();
    }

    Result CopyAlbumFile(StorageId storage, const FileId &fileId) {
        R_UNLESS(!IsReserved(fileId, writeReserve), 0x2ece);
        R_TRY(fileId.Verify());
        R_UNLESS(config::StorageValid(storage), 0x1ace);
        R_TRY(MountAlbum(fileId.storage));
        R_TRY(MountAlbum(storage));
        R_UNLESS(storage != fileId.storage, 0x1ace);
        FileId newFileId = fileId;
        newFileId.storage = storage;
        printf("%s -> %s\n", fileId.GetFilePath().c_str(), newFileId.GetFilePath().c_str());
        return ResultSuccess();
    }

    Result GetAutoSavingStorage(StorageId *out) {
        StorageId storage = GetPrimaryStorage();
        R_UNLESS(MountAlbum(storage).IsSuccess(), capsrv::ResultFailedToMountImageDirectory());

        *out = storage;
        return ResultSuccess();
    }

    Result IsAlbumMounted(bool *out, StorageId storage) {
        R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());

        *out = g_mountStatus[storage];
        return ResultSuccess();
    }

    Result MountAlbum(StorageId storage) {
        R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
        R_UNLESS(!g_mountStatus[storage], ResultSuccess());

        const char *customDirectory = config::GetCustomDirectoryPath();
        if (storage == StorageId::Sd && customDirectory) {
            return MountCustomImageDirectory(customDirectory);
        }
        Result rc = MountImageDirectory(storage);

        if (rc.IsSuccess())
            g_mountStatus[storage] = true;
        return rc;
    }

    Result UnmountAlbum(StorageId storage) {
        R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
        R_UNLESS(g_mountStatus[storage % 2], ResultSuccess());

        Result rc = UnmountImageDirectory(storage);

        if (rc.IsSuccess())
            g_mountStatus[storage] = false;
        return rc;
    }

    Result GetAlbumCache(CapsAlbumCache *out, StorageId storage, ContentType type) {
        R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
        R_UNLESS(config::SupportsType(type), capsrv::ResultInvalidContentType());

        *out = g_storage.position[storage].cache[type];

        return ResultSuccess();
    }

    Result SaveAlbumScreenShotFile(const u8 *buffer, u64 size, const FileId &fileId) {
        R_UNLESS(!IsReserved(fileId, readReserve), 0xbcce);
        R_UNLESS(!IsReserved(fileId, writeReserve), 0xbcce);
        R_TRY(fileId.Verify());
        R_TRY(MountAlbum(fileId.storage));
        R_TRY(g_storage.CanSave(fileId.storage, fileId.type));
        FILE *f = fopen(fileId.GetFilePath().c_str(), "wb");
        fwrite(buffer, sizeof(u8), size, f);
        fclose(f);
        g_storage.Increment(fileId.storage, fileId.type);
        return ResultSuccess();
    }

}
