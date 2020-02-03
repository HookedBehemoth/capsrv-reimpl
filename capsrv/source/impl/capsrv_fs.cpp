#include "capsrv_fs.hpp"
#include "../capsrv_config.hpp"
#include "../capsrv_util.hpp"

#include <map>
#include <string>
#include <filesystem>
#include <machine/endian.h>

namespace ams::capsrv::impl {

    struct Storage {
        CapsAlbumCache usage[4];
    };

    constexpr const char* mountNames[] =
    {
        [StorageId::Nand] = "NA",
        [StorageId::Sd] =   "SD"
    };

    constexpr const char* fileExtensions[] =
    {
        [0] = ".jpg",
        [1] = ".mp4"
    };
    
    namespace {
        bool g_mountStatus[2] = { false, false };
        Storage g_storage[2] = { 0 };
        os::Mutex g_mutex;

        void GetFolderPath(size_t *written, char* buf, size_t max, const FileId &fileId) {
            *written = std::snprintf(buf, max, "%s:/%04hd/%02hhd/%02hhd/",
                mountNames[fileId.storage],
                fileId.datetime.year,
                fileId.datetime.month,
                fileId.datetime.day
            );
        }

        void GetFileName(size_t *written, char* buf, size_t max, const FileId &fileId) {
            u64 aes[2] = {0};
            u64 lower, upper;
            util::aes128EncryptU64(aes, fileId.applicationId);
            lower = __bswap64(aes[0]);
            upper = __bswap64(aes[1]);

            *written = std::snprintf(buf, max, "%04d%02d%02d%02d%02d%02d%02d-%ld%ld%s",
                fileId.datetime.year,
                fileId.datetime.month,
                fileId.datetime.day,
                fileId.datetime.hour,
                fileId.datetime.minute,
                fileId.datetime.second,
                fileId.datetime.id,
                lower, upper,
                fileExtensions[fileId.type % 2]
            );
        }

        void GetFilePath(size_t *written, char* buf, size_t maxLength, const FileId &fileId) {
            *written = 0;
            GetFolderPath(written, buf, maxLength, fileId);
            size_t folderNameLength = *written;
            GetFileName(written, buf + folderNameLength, maxLength - folderNameLength, fileId);
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
                //std::abort();
            }

            return ResultSuccess();
        }

        Result MountCustomImageDirectory(const char* path) {
            if (nullptr)
                return fs::ResultNullptrArgument();

            // TODO: Mount Host PC when TMA is a thing.
            return fs::ResultHostFileSystemCorrupted();
        }

        Result UnmountImageDirectory(StorageId storage) {
            // TODO: Unmount Host PC when TMA is a thing.
            return fsdevUnmountDevice(mountNames[storage]);
        }

        const char* GetMountName(StorageId storage) {
            return mountNames[storage];
        }
    }

    Result DeleteAlbumFile(const FileId &fileId) {
        R_UNLESS(config::StorageValid(fileId.storage), capsrv::ResultInvalidStorageId());
        char path[69];
        size_t written;
        GetFilePath(&written, path, 69, fileId);

        std::scoped_lock lk{g_mutex};
        /* TODO */
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
        std::scoped_lock lk{g_mutex};

        R_UNLESS(!g_mountStatus[storage], ResultSuccess());
        R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());

        const char* customDirectory = config::GetCustomDirectoryPath();
        if (storage == StorageId::Sd && customDirectory) {
            return MountCustomImageDirectory(customDirectory);
        }
        return MountImageDirectory(storage);

        g_mountStatus[storage] = true;
        return ResultSuccess();
    }

    Result UnmountAlbum(StorageId storage) {
        std::scoped_lock lk{g_mutex};

        R_UNLESS(g_mountStatus[storage], ResultSuccess());
        R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
        
        return UnmountImageDirectory(storage);

        g_mountStatus[storage] = false;
        return ResultSuccess();
    }

    Result GetAlbumCache(CapsAlbumCache *out, StorageId storage, ContentType type) {
        R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
        R_UNLESS(config::SupportsType(type), capsrv::ResultInvalidContentType());

        *out = g_storage[storage].usage[type];

        return ResultSuccess();
    }

}