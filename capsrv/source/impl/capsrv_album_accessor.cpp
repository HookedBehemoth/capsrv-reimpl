#include "../capsrv_settings.hpp"
#include "capsrv_album_accessor.hpp"
#include <stratosphere.hpp>
#include <mutex>

namespace ams::capsrv::impl {

    namespace {
        os::Mutex g_mutex;
        Settings g_Settings;
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

}