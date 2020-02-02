#include "capsrv_manager.hpp"
#include "../capsrv_file_id_generator.hpp"
#include "../capsrv_settings.hpp"

#include <mutex>

namespace ams::capsrv::impl {

    struct Storage {
        CapsAlbumCache usage[4];
    };

    namespace {
        os::Mutex g_mutex;
        Settings g_Settings;
        bool g_mountStatus[2];
        Storage g_storage[2];
        FileIdGenerator g_FileIdGenerator;
    }

    Result InitializeAlbumAccessor() {
        g_Settings.Initialize();
        return ResultSuccess();
    }

    void FinalizeAlbumAccessor() {

    }

    StorageId GetPrimaryStorage() {
        SetSysPrimaryAlbumStorage storage;
        Result rc = setsysGetPrimaryAlbumStorage(&storage);
        if (R_FAILED(rc))
            std::abort();

        return StorageId(storage);
    }

    Result GetAutoSavingStorage(StorageId *out) {
        StorageId storage = GetPrimaryStorage();
        Result rc = MountAlbum(storage);
        if (rc.IsFailure())
            return capsrv::ResultFailedToMountImageDirectory();

        *out = storage;
        return ResultSuccess();
    }

    Result GenerateCurrentAlbumFileId(FileId *out, u64 appId, ContentType type) {
        FileId fileId;
        return g_FileIdGenerator.GenerateFileId(&fileId, appId, type);
    }

    Result GetAlbumFileCount(u64* outCount, StorageId storageId) {
        u64 count = 0;
        Result res = 0;
        {
            std::scoped_lock lk(g_mutex);

        }
        if (res.IsSuccess() && outCount)
            *outCount = count;
        return res;
    }

    Result GetAlbumFileList(void* ptr, u64 size, u64* outCount, StorageId storageId) {
        u64 count = 0;
        Result res = 0;
        {
            std::scoped_lock lk(g_mutex);

        }
        if (res.IsSuccess() && outCount)
            *outCount = count;
        return res;
    }

    Result MountAlbum(StorageId storage) {
        R_TRY(g_Settings.MountAlbum(storage));
        g_mountStatus[storage] = true;
        return ResultSuccess();
    }

    Result UnmountAlbum(StorageId storage) {
        R_TRY(g_Settings.UnmountAlbum(storage));
        g_mountStatus[storage] = false;
        return ResultSuccess();
    }

    Result GetAlbumCache(CapsAlbumCache *out, StorageId storage, ContentType type) {
        if (!g_Settings.StorageValid(storage))
            return capsrv::ResultInvalidStorageId();

        if (!g_Settings.SupportsType(type))
            return capsrv::ResultInvalidContentType();

        *out = g_storage[storage].usage[type];

        return ResultSuccess();
    }

}