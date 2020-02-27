#include "capsrv_manager.hpp"

#include <machine/endian.h>

#include <filesystem>
#include <map>
#include <string>
#include "../logger.hpp"

#include "../capsrv_config.hpp"
#include "../image/exif_extractor.hpp"
#include "capsrv_controller.hpp"

namespace ams::capsrv::impl {

    constexpr const char *mountNames[2] = {
        [StorageId::Nand] = "NA",
        [StorageId::Sd] = "SD",
    };

    constexpr const char *mountPoints[2] = {
        [StorageId::Nand] = "NA:/",
        [StorageId::Sd] = "SD:/",
    };

    constexpr const char *fileExtensions[2] = {
        [0] = ".jpg",
        [1] = ".mp4",
    };

    constexpr u64 maxFileSize[2][2] = {
        [StorageId::Nand] = {
            [ContentType::Screenshot] = 0x7D000,
            [ContentType::Movie] = 0x80000000,
        },
        [StorageId::Sd] = {
            [ContentType::Screenshot] = 0x7D000,
            [ContentType::Movie] = 0x80000000,
        },
    };

    namespace {

        ContentStorage g_storage;
        bool g_mountStatus[2] = {false, false};
        struct ReserveItem {
            bool used;
            FileId fileId;
        };
        struct Reserve {
            ReserveItem items[10];
        } readReserve, writeReserve;

        os::Mutex g_mutex;

        bool IsReserved(const FileId &fileId, const Reserve &reserve) {
            for (const auto &item : reserve.items) {
                if (item.used && std::memcmp(&item.fileId, &fileId, sizeof(FileId)) == 0)
                    return true;
            }
            return false;
        }

        Result GetAlbumCacheImpl(CapsAlbumCache *out, StorageId storage, ContentType type) {
            R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
            R_UNLESS(config::SupportsType(type), capsrv::ResultInvalidContentType());

            *out = g_storage.cache[storage][type];

            return ResultSuccess();
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

        Result IsAlbumMountedImpl(bool *out, StorageId storage) {
            R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
            *out = g_mountStatus[storage];
            return ResultSuccess();
        }

        Result MountAlbumImpl(StorageId storage) {
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

        Result UnmountAlbumImpl(StorageId storage) {
            R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
            R_UNLESS(g_mountStatus[storage % 2], ResultSuccess());

            Result rc = UnmountImageDirectory(storage);

            if (rc.IsSuccess())
                g_mountStatus[storage] = false;
            return rc;
        }

        Result GetAutoSavingStorageImpl(StorageId *out) {
            SetSysPrimaryAlbumStorage tmp;
            Result rc = setsysGetPrimaryAlbumStorage(&tmp);

            if (R_FAILED(rc))
                std::abort();

            StorageId storage = StorageId(tmp);
            R_UNLESS(MountAlbumImpl(storage).IsSuccess(), capsrv::ResultFailedToMountImageDirectory());
            *out = storage;
            return ResultSuccess();
        }

        struct ProcessObject {};

        struct CountObject : ProcessObject {
            u64 count;
            u8 flags;
        };

        bool cb_count(const Entry &entry, ProcessObject *ptr) {
            CountObject *user = (CountObject *)ptr;
            if ((user->flags & BIT(0) && (entry.fileId.type % 2 == ContentType::Screenshot)) ||
                (user->flags & BIT(1) && (entry.fileId.type % 2 == ContentType::Movie))) {
                user->count++;
            }
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

            if ((user->flags & BIT(0) && (entry.fileId.type % 2 == ContentType::Screenshot)) ||
                (user->flags & BIT(1) && (entry.fileId.type % 2 == ContentType::Movie))) {
                *user->entries = entry;
                user->entries++;
                user->count++;
            }
            return true;
        }

        struct CacheObject : ProcessObject {
            CapsAlbumCache cache[4];
        };

        bool cb_cache(const Entry &entry, ProcessObject *ptr) {
            CacheObject *user = (CacheObject *)ptr;
            user->cache[entry.fileId.type].count++;
            return true;
        }

        // TODO: Neglect Extra Folder
        Result ProcessImageDirectory(StorageId storage, std::function<bool(const Entry &, ProcessObject *)> callback, ProcessObject *user) {
            std::error_code code;
            for (auto &e : std::filesystem::recursive_directory_iterator(mountPoints[storage], code)) {
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

        Result DeleteAlbumFileImpl(const FileId &fileId) {
            R_UNLESS(!IsReserved(fileId, readReserve), capsrv::ResultFileReserved());
            R_UNLESS(!IsReserved(fileId, writeReserve), capsrv::ResultFileReserved());
            R_TRY(fileId.Verify());
            R_TRY(MountAlbumImpl(fileId.storage));
            std::filesystem::remove(fileId.GetFilePath());
            g_storage.Decrement(fileId.storage, fileId.type);
            return ResultSuccess();
        }

        // TODO: Copy
        Result CopyAlbumFileImpl(StorageId storage, const FileId &fileId) {
            R_UNLESS(!IsReserved(fileId, writeReserve), capsrv::ResultFileTooBig());
            R_TRY(fileId.Verify());
            R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
            R_TRY(MountAlbumImpl(fileId.storage));
            R_TRY(MountAlbumImpl(storage));
            R_UNLESS(storage != fileId.storage, capsrv::ResultInvalidStorageId());
            //FileId newFileId = fileId;
            //newFileId.storage = storage;
            return ResultSuccess();
        }

        Result SaveAlbumScreenShotFileImpl(const u8 *buffer, u64 size, const FileId &fileId) {
            R_UNLESS(!IsReserved(fileId, readReserve), capsrv::ResultFileReserved());
            R_UNLESS(!IsReserved(fileId, writeReserve), capsrv::ResultFileReserved());
            R_TRY(fileId.Verify());
            R_TRY(MountAlbumImpl(fileId.storage));
            R_TRY(g_storage.CanSave(fileId.storage, fileId.type));
            FILE *f = fopen(fileId.GetFilePath().c_str(), "wb");
            fwrite(buffer, sizeof(u8), size, f);
            fclose(f);
            g_storage.Increment(fileId.storage, fileId.type);
            return ResultSuccess();
        }

        /* Do it better... */
        Result IHateNamingStuff(u64 *out_size, void *jpeg, u64 jpeg_size, const FileId &fileId) {
            const char* path = fileId.GetFilePath().c_str();
            FILE *f = fopen(path, "rb");
            WriteLogFile("img", "errno: %s: %s", strerror(errno), path);
            R_UNLESS(f, 0xE02);

            SCOPE_GUARD { fclose(f); };

            int seek0 = fseek(f, 0, SEEK_END);
            WriteLogFile("img", "errno: %s", strerror(errno));
            u64 file_size = ftell(f);
            WriteLogFile("img", "errno: %s", strerror(errno));
            int seek1 = fseek(f, 0, SEEK_SET);
            WriteLogFile("img", "errno: %s", strerror(errno));
            R_UNLESS(file_size > 0, 0x30ce);
            WriteLogFile("img", "size: %ld, seek0: %d, seek1: %d", file_size, seek0, seek1);
            R_UNLESS(file_size <= maxFileSize[fileId.storage][0], 0x2ece);
            R_UNLESS(file_size == fread(jpeg, sizeof(char), file_size, f), capsrv::ResultInvalidArgument());

            *out_size = file_size;
            return ResultSuccess();
        }

        /* Load file. */
        Result LoadImage(CapsScreenShotAttribute *out_attr, void *buf_0, void *buf_1, u64 *out_size, void *work, u64 work_size, const FileId &fileId) {
            if (out_attr) *out_attr = {0};
            if (buf_0) memset(buf_0, 0, 0x400);
            if (buf_1) memset(buf_1, 0, 0x404);

            std::memset(work, 0, work_size);

            R_UNLESS(!IsReserved(fileId, writeReserve), capsrv::ResultFileReserved());
            R_TRY(fileId.Verify());
            R_TRY(MountAlbumImpl(fileId.storage));
            R_UNLESS(config::SupportsType(fileId.type), capsrv::ResultInvalidContentType());

            Result rc = {}; /* TODO: Load image. */
            if ((fileId.type % 2) == ContentType::Screenshot) {
                rc = IHateNamingStuff(out_size, work, work_size, fileId);
            }

            if (R_SUCCEEDED(rc))
                return rc;

            if (out_attr) *out_attr = {0};
            if (buf_0) memset(buf_0, 0, 0x400);
            if (buf_1) memset(buf_1, 0, 0x404);

            return rc;
        }

        Result LoadScreenShotImage(Dimensions *dims, CapsScreenShotAttribute *out_attr, void *buf_0, void *buf_1, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts = {0}) {
            CapsScreenShotAttribute attr{};
            u64 jpegSize = 0;
            /* Load JPEG image. */
            Result rc = LoadImage(out_attr, buf_0, buf_1, &jpegSize, work, work_size, fileId);
            WriteLogFile("img", "0x%x LoadImage: %d, %d", rc, jpegSize, work_size);
            if (R_SUCCEEDED(rc)) {
                /* Convert JPEG image. */
                rc = capsdcDecodeJpeg(1280, 720, &opts, work, jpegSize, img, img_size);
                if (R_SUCCEEDED(rc)) {
                    dims->width = 1280;
                    dims->height = 720;
                    if (out_attr) *out_attr = attr;
                    return rc;
                }
            }
            if (out_attr) *out_attr = {0};
            if (buf_0) memset(buf_0, 0, 0x400);
            if (buf_1) memset(buf_1, 0, 0x404);

            return rc;
        }

        /* Do it better... */
        Result IHateNamingStuffThumbnail(u64 *out_size, void *thumb, u64 thumb_size, const FileId &fileId) {
            const char* path = fileId.GetFilePath().c_str();
            FILE *f = fopen(path, "rb");
            WriteLogFile("img", "errno: %s: %s", strerror(errno), path);
            R_UNLESS(f, 0xE02);

            SCOPE_GUARD { fclose(f); };

            int seek0 = fseek(f, 0, SEEK_END);
            WriteLogFile("img", "errno: %s", strerror(errno));
            u64 file_size = ftell(f);
            WriteLogFile("img", "errno: %s", strerror(errno));
            int seek1 = fseek(f, 0, SEEK_SET);
            WriteLogFile("img", "errno: %s", strerror(errno));
            R_UNLESS(file_size > 0, 0x30ce);
            WriteLogFile("img", "size: %ld, seek0: %d, seek1: %d", file_size, seek0, seek1);
            R_UNLESS(file_size <= maxFileSize[fileId.storage][0], 0x2ece);
            u8 *jpg = (u8*)malloc(file_size);
            R_UNLESS(jpg, capsrv::ResultOutOfMemory());
            R_UNLESS(file_size == fread(jpg, sizeof(char), file_size, f), capsrv::ResultInvalidArgument());

            R_UNLESS(file_size < 0xc, 0xa2ece);

            SCOPE_GUARD { free(jpg); };

            auto bin = ams::image::detail::ExifBinary();
            auto exif = ams::image::ExifExtractor(&bin);

            exif.SetExifData(jpg + 0xc, file_size - 0xc);
            printf("0x%x\n", exif.Analyse());

            u32 temp_size = 0;
            const u8 *temp_nail = exif.ExtractThumbnail(&temp_size);
            R_UNLESS(temp_nail, 0x3cce);
            R_UNLESS(temp_size <= thumb_size, 0x30ce);

            *out_size = thumb_size;
            std::memcpy(thumb, temp_nail, temp_size);

            return ResultSuccess();
        }

        Result LoadThumbnail(CapsScreenShotAttribute *out_attr, void *buf_0, void *buf_1, u64 *out_size, void *work, u64 work_size, const FileId &fileId) {
            if (out_attr) *out_attr = {0};
            if (buf_0) memset(buf_0, 0, 0x400);
            if (buf_1) memset(buf_1, 0, 0x404);

            std::memset(work, 0, work_size);

            R_UNLESS(!IsReserved(fileId, writeReserve), capsrv::ResultFileReserved());
            R_TRY(fileId.Verify());
            R_TRY(MountAlbumImpl(fileId.storage));
            R_UNLESS(config::SupportsType(fileId.type), capsrv::ResultInvalidContentType());

            Result rc = {}; /* TODO: Load thumbnail. */
            if ((fileId.type % 2) == ContentType::Screenshot) {
                rc = IHateNamingStuffThumbnail(out_size, work, work_size, fileId);
            }

            if (R_SUCCEEDED(rc))
                return rc;

            if (out_attr) *out_attr = {0};
            if (buf_0) memset(buf_0, 0, 0x400);
            if (buf_1) memset(buf_1, 0, 0x404);

            return rc;
        }

        Result LoadScreenShotThumbnail(Dimensions *dims, CapsScreenShotAttribute *out_attr, void *buf_0, void *buf_1, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts = {0}) {
            CapsScreenShotAttribute attr{};
            u64 jpegSize = 0;
            /* Load JPEG thumbnail. */
            Result rc = LoadThumbnail(out_attr, buf_0, buf_1, &jpegSize, work, work_size, fileId);
            WriteLogFile("img", "0x%x LoadThumbnail: %d, %d", rc, jpegSize, work_size);
            if (R_SUCCEEDED(rc)) {
                /* Convert JPEG thumbnail. */
                rc = capsdcDecodeJpeg(320, 180, &opts, work, work_size, img, img_size);
                if (R_SUCCEEDED(rc)) {
                    dims->width = 320;
                    dims->height = 180;
                    if (out_attr) *out_attr = attr;
                    return rc;
                }
            }
            if (out_attr) *out_attr = {0};
            if (buf_0) memset(buf_0, 0, 0x400);
            if (buf_1) memset(buf_1, 0, 0x404);

            return rc;
        }

    }

    /* Query operations. */
    Result GetAlbumFileCount(u64 *outCount, StorageId storage, u8 flags) {
        R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
        CountObject countObject = {
            .count = 0,
            .flags = flags,
        };
        Result res = 0;
        {
            std::scoped_lock lk(g_mutex);
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
            .flags = flags,
        };
        Result res = 0;
        {
            std::scoped_lock lk(g_mutex);
            res = ProcessImageDirectory(storage, cb_list, &listObject);
        }
        if (res.IsSuccess() && outCount)
            *outCount = listObject.count;
        return res;
    }

    Result RefreshAlbumCache(StorageId storage) {
        R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
        CacheObject cacheObject{};
        {
            std::scoped_lock lk(g_mutex);
            R_TRY(MountAlbumImpl(storage));
            R_TRY(ProcessImageDirectory(storage, cb_cache, &cacheObject));
        }
        g_storage.Set(storage, ContentType::Screenshot, cacheObject.cache[ContentType::Screenshot]);
        g_storage.Set(storage, ContentType::Movie, cacheObject.cache[ContentType::Movie]);
        g_storage.Set(storage, ContentType::ExtraScreenshot, cacheObject.cache[ContentType::ExtraScreenshot]);
        g_storage.Set(storage, ContentType::ExtraMovie, cacheObject.cache[ContentType::ExtraMovie]);
        return ResultSuccess();
    }

    Result GetAlbumCache(CapsAlbumCache *out, StorageId storage, ContentType type) {
        std::scoped_lock lk(g_mutex);
        return GetAlbumCacheImpl(out, storage, type);
    }

    Result GetAutoSavingStorage(StorageId *out) {
        std::scoped_lock lk(g_mutex);
        return GetAutoSavingStorageImpl(out);
    }

    /* FS modifiers. */
    Result IsAlbumMounted(bool *out, StorageId storage) {
        std::scoped_lock lk(g_mutex);
        return IsAlbumMountedImpl(out, storage);
    }

    Result MountAlbum(StorageId storage) {
        std::scoped_lock lk(g_mutex);
        return MountAlbumImpl(storage);
    }

    Result UnmountAlbum(StorageId storage) {
        std::scoped_lock lk(g_mutex);
        return UnmountAlbumImpl(storage);
    }

    Result DeleteAlbumFile(const FileId &fileId) {
        std::scoped_lock lk(g_mutex);
        return DeleteAlbumFileImpl(fileId);
    }

    Result CopyAlbumFile(StorageId storage, const FileId &fileId) {
        std::scoped_lock lk(g_mutex);
        return CopyAlbumFileImpl(storage, fileId);
    }

    Result SaveAlbumScreenShotFile(const u8 *buffer, u64 size, const FileId &fileId) {
        std::scoped_lock lk(g_mutex);
        return SaveAlbumScreenShotFileImpl(buffer, size, fileId);
    }

    /* Load file. */
    Result LoadAlbumFile(void *ptr, u64 size, u64 *outSize, const FileId &fileId) {
        std::scoped_lock lk(g_mutex);
        return LoadImage(nullptr, nullptr, nullptr, outSize, ptr, size, fileId);
    }

    Result LoadAlbumFileThumbnail(void *ptr, u64 size, u64 *outSize, const FileId &fileId) {
        std::scoped_lock lk(g_mutex);
        return LoadThumbnail(nullptr, nullptr, nullptr, outSize, ptr, size, fileId);
    }

    Result LoadAlbumScreenShotImage(u64 *width, u64 *height, void *work, u64 work_size, void *img, u64 img_size, const FileId &fileId) {
        Result rc{};
        Dimensions dims{};
        {
            std::scoped_lock lk(g_mutex);
            rc = LoadScreenShotImage(&dims, nullptr, nullptr, nullptr, img, img_size, work, work_size, fileId);
        }
        if (R_SUCCEEDED(rc)) {
            if (width) *width = dims.width;
            if (height) *height = dims.height;
        }
        std::memset(work, 0, work_size);
        return rc;
    }

    Result LoadAlbumScreenShotThumbnailImage(u64 *width, u64 *height, void *work, u64 work_size, void *img, u64 img_size, const FileId &fileId) {
        Result rc{};
        Dimensions dims{};
        {
            std::scoped_lock lk(g_mutex);
            rc = LoadScreenShotThumbnail(&dims, nullptr, nullptr, nullptr, img, img_size, work, work_size, fileId);
        }
        if (R_SUCCEEDED(rc)) {
            if (width) *width = dims.width;
            if (height) *height = dims.height;
        }
        std::memset(work, 0, work_size);
        return rc;
    }

    Result LoadAlbumScreenShotImageEx(u64 *width, u64 *height, void *work, u64 work_size, void *img, u64 img_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        {
            std::scoped_lock lk(g_mutex);
            rc = LoadScreenShotImage(&dims, nullptr, nullptr, nullptr, img, img_size, work, work_size, fileId, opts);
        }
        if (R_SUCCEEDED(rc)) {
            if (width) *width = dims.width;
            if (height) *height = dims.height;
        }
        std::memset(work, 0, work_size);
        return rc;
    }
    Result LoadAlbumScreenShotThumbnailImageEx(u64 *width, u64 *height, void *work, u64 work_size, void *img, u64 img_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        {
            std::scoped_lock lk(g_mutex);
            rc = LoadScreenShotThumbnail(&dims, nullptr, nullptr, nullptr, img, img_size, work, work_size, fileId, opts);
        }
        if (R_SUCCEEDED(rc)) {
            if (width) *width = dims.width;
            if (height) *height = dims.height;
        }
        std::memset(work, 0, work_size);
        return rc;
    }

    Result LoadAlbumScreenShotImageEx0(CapsScreenShotAttribute *out_attr, u64 *width, u64 *height, void *work, u64 work_size, void *img, u64 img_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        CapsScreenShotAttribute attr{};
        {
            std::scoped_lock lk(g_mutex);
            rc = LoadScreenShotImage(&dims, &attr, nullptr, nullptr, img, img_size, work, work_size, fileId, opts);
        }
        if (R_SUCCEEDED(rc)) {
            if (width) *width = dims.width;
            if (height) *height = dims.height;
            if (out_attr) *out_attr = attr;
        }
        std::memset(work, 0, work_size);
        return rc;
    }
    Result LoadAlbumScreenShotThumbnailImageEx0(CapsScreenShotAttribute *out_attr, u64 *width, u64 *height, void *work, u64 work_size, void *img, u64 img_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        CapsScreenShotAttribute attr{};
        {
            std::scoped_lock lk(g_mutex);
            rc = LoadScreenShotThumbnail(&dims, &attr, nullptr, nullptr, img, img_size, work, work_size, fileId, opts);
        }
        if (R_SUCCEEDED(rc)) {
            if (width) *width = dims.width;
            if (height) *height = dims.height;
            if (out_attr) *out_attr = attr;
        }
        std::memset(work, 0, work_size);
        return rc;
    }

    Result LoadAlbumScreenShotImageEx1(LoadAlbumScreenShotImageOutput *out, void *work, u64 work_size, void *img, u64 img_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        CapsScreenShotAttribute attr{};
        {
            std::scoped_lock lk(g_mutex);
            rc = LoadScreenShotImage(&dims, &attr, out->unk_x50, nullptr, img, img_size, work, work_size, fileId, opts);
        }
        if (R_SUCCEEDED(rc) && out) {
            out->width = dims.width;
            out->height = dims.height;
            out->attr = attr;
        }
        std::memset(work, 0, work_size);
        return rc;
    }
    Result LoadAlbumScreenShotThumbnailImageEx1(LoadAlbumScreenShotImageOutput *out, void *work, u64 work_size, void *img, u64 img_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        CapsScreenShotAttribute attr{};
        {
            std::scoped_lock lk(g_mutex);
            rc = LoadScreenShotThumbnail(&dims, &attr, out->unk_x50, nullptr, img, img_size, work, work_size, fileId, opts);
        }
        if (R_SUCCEEDED(rc) && out) {
            out->width = dims.width;
            out->height = dims.height;
            out->attr = attr;
        }
        std::memset(work, 0, work_size);
        return rc;
    }

    Result LoadAlbumScreenShotImageByAruid(LoadAlbumScreenShotImageOutputForApplication *out, void *work, u64 work_size, void *img, u64 img_size, u64 aruid, const CapsApplicationAlbumFileEntry &appFileEntry, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        CapsScreenShotAttribute attr{};
        Entry entry{};
        control::GetAlbumEntryFromApplicationAlbumEntryAruid(&entry, (ApplicationEntry *)&appFileEntry.entry, aruid);
        {
            std::scoped_lock lk(g_mutex);
            rc = LoadScreenShotImage(&dims, &attr, nullptr, &out->appdata, img, img_size, work, work_size, entry.fileId, opts);
        }
        if (R_SUCCEEDED(rc) && out) {
            out->width = dims.width;
            out->height = dims.height;
            (out->attr).unk_x0 = attr.orientation;
            (out->attr).unk_x4 = attr.unk_x0;
            (out->attr).unk_x5 = attr.reserved[2];
            (out->attr).unk_x6 = attr.reserved[1];
            (out->attr).pad = '\0';
            (out->attr).unk_x8 = attr.reserved[0];
            (out->attr).unk_xc = attr.unk_xc;
            (out->attr).unk_x10 = attr.unk_x10;
            (out->attr).unk_x14 = attr.unk_x14;
            (out->attr).unk_x18 = attr.unk_x18;
            (out->attr).unk_x1c = attr.unk_x1c;
            (out->attr).unk_x20 = attr.unk_x20;
            (out->attr).unk_x22 = attr.unk_x22;
            (out->attr).unk_x24 = attr.unk_x24;
            (out->attr).unk_x26 = attr.unk_x26;
            (out->attr).reserved[1] = 0;
            (out->attr).reserved[2] = 0;
            (out->attr).reserved[0] = 0;
        }
        return rc;
    }
    Result LoadAlbumScreenShotThumbnailImageByAruid(LoadAlbumScreenShotImageOutputForApplication *out, void *work, u64 work_size, void *img, u64 img_size, u64 aruid, const CapsApplicationAlbumFileEntry &appFileEntry, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        CapsScreenShotAttribute attr{};
        Entry entry{};
        control::GetAlbumEntryFromApplicationAlbumEntryAruid(&entry, (ApplicationEntry *)&appFileEntry.entry, aruid);
        {
            std::scoped_lock lk(g_mutex);
            rc = LoadScreenShotThumbnail(&dims, &attr, nullptr, &out->appdata, img, img_size, work, work_size, entry.fileId, opts);
        }
        if (R_SUCCEEDED(rc) && out) {
            out->width = dims.width;
            out->height = dims.height;
            (out->attr).unk_x0 = attr.orientation;
            (out->attr).unk_x4 = attr.unk_x0;
            (out->attr).unk_x5 = attr.reserved[2];
            (out->attr).unk_x6 = attr.reserved[1];
            (out->attr).pad = '\0';
            (out->attr).unk_x8 = attr.reserved[0];
            (out->attr).unk_xc = attr.unk_xc;
            (out->attr).unk_x10 = attr.unk_x10;
            (out->attr).unk_x14 = attr.unk_x14;
            (out->attr).unk_x18 = attr.unk_x18;
            (out->attr).unk_x1c = attr.unk_x1c;
            (out->attr).unk_x20 = attr.unk_x20;
            (out->attr).unk_x22 = attr.unk_x22;
            (out->attr).unk_x24 = attr.unk_x24;
            (out->attr).unk_x26 = attr.unk_x26;
            (out->attr).reserved[1] = 0;
            (out->attr).reserved[2] = 0;
            (out->attr).reserved[0] = 0;
        }
        return rc;
    }

}
