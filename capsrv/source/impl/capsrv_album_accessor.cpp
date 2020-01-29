#include "capsrv_album_accessor.hpp"
#include "../capsrv_settings.hpp"

#include <stratosphere/os.hpp>
#include <switch.h>
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
    }

    Result InitializeAlbumAccessor() {
        g_Settings.Initialize();
        return ResultSuccess();
    }

    void FinalizeAlbumAccessor() {

    }

    Result GetAlbumFileCount(u64* outCount, const StorageId storageId) {
        u64 count = 0;
        Result res = 0;
        {
            std::scoped_lock lk(g_mutex);

        }
        if (res.IsSuccess() && outCount)
            *outCount = count;
        return res;
    }

    Result GetAlbumFileList(void* ptr, u64 size, u64* outCount, const StorageId storageId) {
        u64 count = 0;
        Result res = 0;
        {
            std::scoped_lock lk(g_mutex);

        }
        if (res.IsSuccess() && outCount)
            *outCount = count;
        return res;
    }

    Result LoadAlbumFile(void* ptr, u64 size, u64* outSize, const FileId &fileId) {
        *outSize = 0;
        return ResultSuccess();
    }

    Result LoadAlbumScreenShotThumbnail(u64 *width, u64 *height, CapsScreenShotAttribute *attr, void* dat0, void* dat1, void* dat2, void* image, u64 imageSize, void* work, u64 workSize, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        return 100;
    }

    Result MountAlbum(const StorageId storage) {
        return g_Settings.MountAlbum(storage);
    }

    Result UnmountAlbum(const StorageId storage) {
        return g_Settings.UnmountAlbum(storage);
    }

    Result GetAlbumCache(CapsAlbumCache *out, const StorageId storage, const ContentType type) {
        if (!g_Settings.StorageValid(storage))
            return capsrv::ResultInvalidStorageId();

        if (!g_Settings.SupportsType(type))
            return capsrv::ResultInvalidContentType();

        *out = g_storage[(u8)storage].usage[(u8)type];

        return ResultSuccess();
    }

}