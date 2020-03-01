#include "capsrv_manager.hpp"

#include <machine/endian.h>

#include "../capsrv_config.hpp"
#include "../image/exif_extractor.hpp"
#include "../logger.hpp"
#include "capsrv_controller.hpp"

#define WORK_MEMORY_SIZE 0x51000

namespace ams::capsrv::impl {

    namespace {

        ContentStorage g_storage;
        FsFileSystem g_fsFs[2]{};
        bool g_mountStatus[2] = {false, false};
        struct ReserveItem {
            bool used;
            FileId fileId;
        };
        struct Reserve {
            ReserveItem items[10];
        } readReserve, writeReserve;

        os::Mutex g_mutex;
        u8 g_workMemory[WORK_MEMORY_SIZE];

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

            out->count = g_storage.cache[storage][type].count;
            out->unk_x8 = 0;

            return ResultSuccess();
        }

        Result MountImageDirectory(StorageId storage) {
            R_TRY(fsOpenImageDirectoryFileSystem(&g_fsFs[storage], FsImageDirectoryId(storage)));
            g_mountStatus[storage] = true;
            return ResultSuccess();
        }

        Result MountCustomImageDirectory(const char *path) {
            if (nullptr)
                return fs::ResultNullptrArgument();

            /* TODO: Mount Host PC when TMA is a thing. */
            return fs::ResultHostFileSystemCorrupted();
        }

        Result UnmountImageDirectory(StorageId storage) {
            fsFsClose(&g_fsFs[storage]);

            g_mountStatus[storage] = false;
            return ResultSuccess();
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
            return MountImageDirectory(storage);
        }

        Result UnmountAlbumImpl(StorageId storage) {
            R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
            R_UNLESS(g_mountStatus[storage % 2], ResultSuccess());
            /* TODO: Close movie streams. */
            return UnmountImageDirectory(storage);
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

        Result _fsFsOpenCommon(FsFileSystem *fs, const char *path, size_t path_size, u32 flags, Service *out, u32 cmd_id) {
            return serviceDispatchIn(&fs->s, cmd_id, flags,
                                     .buffer_attrs = {SfBufferAttr_HipcPointer | SfBufferAttr_In},
                                     .buffers = {{path, path_size}},
                                     .out_num_objects = 1,
                                     .out_objects = out, );
        }

        Result fsFsOpenFileSmoll(FsFileSystem *fs, const char *path, size_t path_size, u32 mode, FsFile *out) {
            return _fsFsOpenCommon(fs, path, path_size, mode, &out->s, 8);
        }

        Result fsFsOpenDirectorySmoll(FsFileSystem *fs, const char *path, size_t path_size, u32 mode, FsDir *out) {
            return _fsFsOpenCommon(fs, path, path_size, mode, &out->s, 9);
        }

        /* TODO: Extra contents. */
        Result ProcessImageDirectory(StorageId storage, std::function<bool(const Entry &)> callback) {
            char tmp_path[0x10] = "/";
            /* Open root directory. Only read directories. */
            FsDir rootDir;
            R_TRY(fsFsOpenDirectorySmoll(&g_fsFs[storage], tmp_path, 0x10, FsDirOpenMode_ReadDirs, &rootDir));
            /* Close directory on scope exit. */
            ON_SCOPE_EXIT { fsDirClose(&rootDir); };

            /* Count directory entries. */
            s64 count;
            R_TRY(fsDirGetEntryCount(&rootDir, &count));

            /* Read directory entries. */
            s64 readCount;
            FsDirectoryEntry yearEntries[count];
            R_TRY(fsDirRead(&rootDir, &readCount, count, yearEntries));

            for (auto &yearEntry : yearEntries) {
                /* Redundant? */
                if (yearEntry.type != FsDirEntryType_Dir)
                    continue;
                /* Only read directories with 'valid' year names. */
                int year = atoi(yearEntry.name);
                if (year < 2000 || 10000 < year)
                    continue;

                /* Copy trailing null terminator. */
                for (int i = 0; i < 5; i++)
                    tmp_path[i + 1] = yearEntry.name[i];

                /* Open year directory. Only read directories. */
                FsDir yearDir;
                R_TRY(fsFsOpenDirectorySmoll(&g_fsFs[storage], tmp_path, 0x10, FsDirOpenMode_ReadDirs, &yearDir));
                /* Close directory on scope exit. */
                ON_SCOPE_EXIT { fsDirClose(&yearDir); };

                /* Count directory entries. */
                R_TRY(fsDirGetEntryCount(&yearDir, &count));

                /* Read directory entries. */
                FsDirectoryEntry monthEntries[count];
                R_TRY(fsDirRead(&yearDir, &readCount, count, monthEntries));

                for (auto &monthEntry : monthEntries) {
                    /* Redundant? */
                    if (monthEntry.type != FsDirEntryType_Dir)
                        continue;
                    /* Only read directories with 'valid' month names. */
                    int month = atoi(monthEntry.name);
                    if (month < 1 || 12 < month)
                        continue;

                    /* Copy trailing null terminator. */
                    tmp_path[5] = '/';
                    for (int i = 0; i < 3; i++)
                        tmp_path[i + 6] = monthEntry.name[i];

                    /* Open month directory. Only read directories. */
                    FsDir monthDir;
                    R_TRY(fsFsOpenDirectorySmoll(&g_fsFs[storage], tmp_path, 0x10, FsDirOpenMode_ReadDirs, &monthDir));
                    /* Close directory on scope exit. */
                    ON_SCOPE_EXIT { fsDirClose(&monthDir); };

                    /* Count directory entries. */
                    R_TRY(fsDirGetEntryCount(&monthDir, &count));

                    /* Read directory entries. */
                    FsDirectoryEntry dayEntries[count];
                    R_TRY(fsDirRead(&monthDir, &readCount, count, dayEntries));

                    for (auto &dayEntry : dayEntries) {
                        /* Redundant? */
                        if (dayEntry.type != FsDirEntryType_Dir)
                            continue;
                        /* Only read directories with 'valid' day names. */
                        int day = atoi(dayEntry.name);
                        if (day < 1 || 31 < day)
                            continue;

                        tmp_path[8] = '/';
                        /* Copy trailing null terminator. */
                        for (int i = 0; i < 3; i++)
                            tmp_path[i + 9] = dayEntry.name[i];

                        /* Open day directory. Only read files. */
                        FsDir dayDir;
                        R_TRY(fsFsOpenDirectorySmoll(&g_fsFs[storage], tmp_path, 0x10, FsDirOpenMode_ReadFiles, &dayDir));
                        /* Close directory on scope exit. */
                        ON_SCOPE_EXIT { fsDirClose(&dayDir); };

                        /* Count directory entries. */
                        R_TRY(fsDirGetEntryCount(&dayDir, &count));

                        /* Read directory entries. */
                        FsDirectoryEntry fileEntries[count];
                        R_TRY(fsDirRead(&dayDir, &readCount, count, fileEntries));

                        for (auto &fileEntry : fileEntries) {
                            /* Redundant? */
                            if (fileEntry.type != FsDirEntryType_File)
                                continue;

                            FileId fileId{};
                            Result rc = FileId::FromString(&fileId, storage, fileEntry.name);
                            Entry entry = {
                                .size = fileEntry.file_size,
                                .fileId = fileId,
                            };
                            if (R_SUCCEEDED(rc)) {
                                R_UNLESS(callback(entry), ResultSuccess());
                            }
                        }
                    }
                }
            }
            return ResultSuccess();
        }

        Result DeleteAlbumFileImpl(const FileId &fileId) {
            R_UNLESS(!IsReserved(fileId, readReserve), capsrv::ResultFileReserved());
            R_UNLESS(!IsReserved(fileId, writeReserve), capsrv::ResultFileReserved());
            R_TRY(fileId.Verify());
            R_TRY(MountAlbumImpl(fileId.storage));

            u64 path_length = fileId.GetPathLength();
            char path[path_length];
            fileId.GetFilePath(path, path_length);

            R_TRY(fsFsDeleteFile(&g_fsFs[fileId.storage], path));
            g_storage.Decrement(fileId.storage, fileId.type);
            return ResultSuccess();
        }

#define EnsureDirectory(delimiter)                                         \
    {                                                                      \
        path[delimiter] = '\0';                                            \
        Result rc = fsFsCreateDirectory(&g_fsFs[storage], path);    \
        if (rc.GetValue() == 0x402) {                                      \
            FsDirEntryType type;                                           \
            R_TRY(fsFsGetEntryType(&g_fsFs[storage], path, &type)); \
            R_UNLESS(type == FsDirEntryType_Dir, 0x90ce);                  \
        }                                                                  \
        path[delimiter] = '/';                                             \
    }
        Result CreateAlbumFileImpl(StorageId storage, char* path, u64 path_size, bool isExtra, s64 size, FsFile *file) {
            if (isExtra) {
                EnsureDirectory(6);
                EnsureDirectory(23);
                EnsureDirectory(28);
                EnsureDirectory(31);
                EnsureDirectory(34);
            } else {
                EnsureDirectory(5);
                EnsureDirectory(8);
                EnsureDirectory(11);
            }
            Result rc = fsFsCreateFile(&g_fsFs[storage], path, size, 0);
            if (R_SUCCEEDED(rc)) {
                rc = fsFsOpenFileSmoll(&g_fsFs[storage], path, path_size, FsOpenMode_Write, file);
                if (R_FAILED(rc)) {
                    fsFsDeleteFile(&g_fsFs[storage], path);
                }
            } else {
                if (rc.GetValue() == 0x402) {
                    rc = fsFsOpenFileSmoll(&g_fsFs[storage], path, path_size, FsOpenMode_Write, file);
                    if (R_SUCCEEDED(rc)) {
                        rc = fsFileSetSize(file, size);
                        if (R_FAILED(rc)) {
                            fsFsDeleteFile(&g_fsFs[storage], path);
                        }
                    }
                }
            }

            return rc;
        }

        Result CopyAlbumFileImpl(StorageId storage, const FileId &fileId) {
            R_UNLESS(!IsReserved(fileId, writeReserve), capsrv::ResultFileTooBig());
            R_TRY(fileId.Verify());
            R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
            R_TRY(MountAlbumImpl(fileId.storage));
            R_TRY(MountAlbumImpl(storage));
            R_UNLESS(storage != fileId.storage, capsrv::ResultInvalidStorageId());

            u64 path_length = fileId.GetPathLength();
            char path[path_length];
            fileId.GetFilePath(path, path_length);

            FsFile srcFile;
            R_TRY(fsFsOpenFileSmoll(&g_fsFs[fileId.storage], path, path_length, FsOpenMode_Read, &srcFile));
            ON_SCOPE_EXIT { fsFileClose(&srcFile); };

            s64 size;
            R_TRY(fsFileGetSize(&srcFile, &size));

            FsFile dstFile;
            R_TRY(CreateAlbumFileImpl(storage, path, path_length, fileId.IsExtra(), size, &dstFile));
            auto file_failure_guard = SCOPE_GUARD { fsFsDeleteFile(&g_fsFs[storage], path); };
            ON_SCOPE_EXIT { fsFileClose(&dstFile); };

            s64 offset = 0;
            u64 readSize;
            while (offset < size) {
                R_TRY(fsFileRead(&srcFile, offset, g_workMemory, WORK_MEMORY_SIZE, FsReadOption_None, &readSize));
                R_TRY(fsFileWrite(&dstFile, offset, g_workMemory, readSize, FsWriteOption_None));
                offset += readSize;
            }

            fsFileFlush(&dstFile);

            file_failure_guard.Cancel();
            g_storage.Increment(storage, fileId.type);
            return ResultSuccess();
        }

        Result GetAlbumFileSizeImpl(u64 *out, const FileId &fileId) {
            R_UNLESS(!IsReserved(fileId, writeReserve), capsrv::ResultFileTooBig());
            R_TRY(fileId.Verify());
            R_UNLESS(config::StorageValid(fileId.storage), capsrv::ResultInvalidStorageId());
            R_TRY(MountAlbumImpl(fileId.storage));

            u64 path_length = fileId.GetPathLength();
            char path[path_length];
            fileId.GetFilePath(path, path_length);

            FsFile file;
            R_TRY(fsFsOpenFileSmoll(&g_fsFs[fileId.storage], path, path_length, FsOpenMode_Read, &file));
            ON_SCOPE_EXIT { fsFileClose(&file); };

            s64 size;
            R_TRY(fsFileGetSize(&file, &size));

            R_UNLESS(config::GetMaxFileSize(fileId.storage, fileId.type) >= size, capsrv::ResultFileTooBig());

            *out = static_cast<u64>(size);
            return ResultSuccess();
        }

        Result SaveAlbumScreenShotFileImpl(const u8 *buffer, u64 size, const FileId &fileId) {
            R_UNLESS(!IsReserved(fileId, readReserve), capsrv::ResultFileReserved());
            R_UNLESS(!IsReserved(fileId, writeReserve), capsrv::ResultFileReserved());
            R_TRY(fileId.Verify());
            R_TRY(MountAlbumImpl(fileId.storage));
            R_TRY(g_storage.CanSave(fileId.storage, fileId.type));

            /* TODO: Fix Exif. */

            u64 path_length = fileId.GetPathLength();
            char path[path_length];
            fileId.GetFilePath(path, path_length);

            FsFile dstFile;
            R_TRY(CreateAlbumFileImpl(fileId.storage, path, path_length, fileId.IsExtra(), size, &dstFile));
            auto file_failure_guard = SCOPE_GUARD { fsFsDeleteFile(&g_fsFs[fileId.storage], path); };
            ON_SCOPE_EXIT { fsFileClose(&dstFile); };

            R_TRY(fsFileSetSize(&dstFile, size));
            R_TRY(fsFileWrite(&dstFile, 0, buffer, size, FsWriteOption_Flush));

            file_failure_guard.Cancel();
            g_storage.Increment(fileId.storage, fileId.type);
            return ResultSuccess();
        }

        /* Do it better... */
        Result IHateNamingStuff(u64 *out_size, void *jpeg, u64 jpeg_size, const FileId &fileId) {
            FsFile file;

            u64 path_length = fileId.GetPathLength();
            char path[path_length];
            fileId.GetFilePath(path, path_length);

            R_TRY(fsFsOpenFileSmoll(&g_fsFs[fileId.storage], path, path_length, FsOpenMode_Read, &file));
            ON_SCOPE_EXIT { fsFileClose(&file); };

            s64 size;
            R_TRY(fsFileGetSize(&file, &size));

            R_UNLESS(size > 0, capsrv::ResultInvalidFileData());
            R_UNLESS(size <= config::GetMaxFileSize(fileId.storage, fileId.type), capsrv::ResultFileTooBig());
            R_UNLESS(static_cast<u64>(size) <= jpeg_size, capsrv::ResultInvalidArgument());

            R_TRY(fsFileRead(&file, 0, jpeg, size, 0, out_size));

            R_UNLESS(static_cast<u64>(size) == *out_size, capsrv::ResultInvalidArgument());

            return ResultSuccess();
        }

        /* Load file. */
        Result LoadImage(CapsScreenShotAttribute *out_attr, void *buf_0, void *buf_1, u64 *out_size, void *work, u64 work_size, const FileId &fileId) {
            if (out_attr)
                *out_attr = {0};
            if (buf_0)
                std::memset(buf_0, 0, 0x400);
            if (buf_1)
                std::memset(buf_1, 0, 0x404);

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

            if (out_attr)
                *out_attr = {0};
            if (buf_0)
                std::memset(buf_0, 0, 0x400);
            if (buf_1)
                std::memset(buf_1, 0, 0x404);

            return rc;
        }

        Result LoadScreenShotImage(Dimensions *dims, CapsScreenShotAttribute *out_attr, void *buf_0, void *buf_1, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts = {0}) {
            CapsScreenShotAttribute attr{};
            u64 jpegSize = 0;
            /* Load JPEG image. */
            Result rc = LoadImage(out_attr, buf_0, buf_1, &jpegSize, work, work_size, fileId);
            if (R_SUCCEEDED(rc)) {
                /* Convert JPEG image. */
                rc = capsdcDecodeJpeg(1280, 720, &opts, work, jpegSize, img, img_size);
                if (R_SUCCEEDED(rc)) {
                    dims->width = 1280;
                    dims->height = 720;
                    if (out_attr)
                        *out_attr = attr;
                    return rc;
                }
            }
            if (out_attr)
                *out_attr = {0};
            if (buf_0)
                std::memset(buf_0, 0, 0x400);
            if (buf_1)
                std::memset(buf_1, 0, 0x404);

            return rc;
        }

        /* Do it better... */
        Result IHateNamingStuffThumbnail(u64 *out_size, void *thumb, u64 thumb_size, const FileId &fileId) {
            FsFile file;

            u64 path_length = fileId.GetPathLength();
            char path[path_length];
            fileId.GetFilePath(path, path_length);

            R_TRY(fsFsOpenFileSmoll(&g_fsFs[fileId.storage], path, path_length, FsOpenMode_Read, &file));
            ON_SCOPE_EXIT { fsFileClose(&file); };

            s64 size;
            R_TRY(fsFileGetSize(&file, &size));

            R_UNLESS(size > 0, capsrv::ResultInvalidFileData());
            R_UNLESS(size <= config::GetMaxFileSize(fileId.storage, fileId.type), capsrv::ResultFileTooBig());

            if (size > 0x11000)
                size = 0x11000;

            R_UNLESS(size <= WORK_MEMORY_SIZE, capsrv::ResultOutOfMemory());

            u64 read_size;
            R_TRY(fsFileRead(&file, 0, g_workMemory, size, 0, &read_size));

            R_UNLESS(static_cast<u64>(size) == read_size, capsrv::ResultInvalidArgument());

            /* TODO: Actually check JFIF header and only pass exif size. */
            R_UNLESS(read_size > 0xc, capsrv::ResultInvalidJFIF());

            auto bin = ams::image::detail::ExifBinary();
            auto exif = ams::image::ExifExtractor(&bin);

            exif.SetExifData(g_workMemory + 0xc, read_size - 0xc);
            R_UNLESS(exif.Analyse(), capsrv::ResultInvalidEXIF());

            u32 temp_size = 0;
            const u8 *temp_nail = exif.ExtractThumbnail(&temp_size);
            R_UNLESS(temp_nail, capsrv::ResultBufferInsufficient());
            R_UNLESS(temp_size <= thumb_size, capsrv::ResultInvalidFileData());

            *out_size = temp_size;
            std::memcpy(thumb, temp_nail, temp_size);

            return ResultSuccess();
        }

        Result LoadThumbnail(CapsScreenShotAttribute *out_attr, void *buf_0, void *buf_1, u64 *out_size, void *work, u64 work_size, const FileId &fileId) {
            if (out_attr)
                *out_attr = {0};
            if (buf_0)
                std::memset(buf_0, 0, 0x400);
            if (buf_1)
                std::memset(buf_1, 0, 0x404);

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

            if (out_attr)
                *out_attr = {0};
            if (buf_0)
                std::memset(buf_0, 0, 0x400);
            if (buf_1)
                std::memset(buf_1, 0, 0x404);

            return rc;
        }

        Result LoadScreenShotThumbnail(Dimensions *dims, CapsScreenShotAttribute *out_attr, void *buf_0, void *buf_1, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts = {0}) {
            CapsScreenShotAttribute attr{};
            u64 jpegSize = 0;
            /* Load JPEG thumbnail. */
            Result rc = LoadThumbnail(out_attr, buf_0, buf_1, &jpegSize, work, work_size, fileId);
            if (R_SUCCEEDED(rc)) {
                /* Convert JPEG thumbnail. */
                rc = capsdcDecodeJpeg(320, 180, &opts, work, jpegSize, img, img_size);
                if (R_SUCCEEDED(rc)) {
                    dims->width = 320;
                    dims->height = 180;
                    if (out_attr)
                        *out_attr = attr;
                    return rc;
                }
            }
            if (out_attr)
                *out_attr = {0};
            if (buf_0)
                std::memset(buf_0, 0, 0x400);
            if (buf_1)
                std::memset(buf_1, 0, 0x404);

            return rc;
        }

    }

    /* Query operations. */
    Result GetAlbumFileCount(u64 *outCount, StorageId storage, u8 flags) {
        R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
        u64 count = 0;
        {
            std::scoped_lock lk(g_mutex);
            R_TRY(ProcessImageDirectory(storage, [&count, flags](const Entry &entry) -> bool {
                for (u8 i = 0; i < 4; i++) {
                    if (flags & BIT(i) && entry.fileId.type == ContentType(i)) {
                        count++;
                        return true;
                    }
                }
                return true;
            }));
        }
        *outCount = count;
        return ResultSuccess();
    }

    Result GetAlbumFileList(void *ptr, u64 size, u64 *outCount, StorageId storage, u8 flags) {
        R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
        Entry *entries = static_cast<Entry *>(ptr);
        u64 count = 0;
        {
            std::scoped_lock lk(g_mutex);
            R_TRY(ProcessImageDirectory(storage, [&](const Entry &entry) -> bool {
                if (size == count)
                    return false;

                for (u8 i = 0; i < 4; i++) {
                    if (flags & BIT(i) && entry.fileId.type == ContentType(i)) {
                        *entries = entry;
                        entries++;
                        count++;
                        return true;
                    }
                }

                return true;
            }));
        }
        *outCount = count;
        return ResultSuccess();
    }

    Result GetAlbumFileListAafeAruid(void *ptr, u64 size, u64 *outCount, DateTime start, DateTime end, ContentType type, u64 aruid) {
        *outCount = 0;
        R_UNLESS(type < 4, capsrv::ResultInvalidContentType());
        R_UNLESS(type != ContentType::ExtraScreenshot, capsrv::ResultInvalidContentType());
        u64 appId;
        R_TRY(control::GetApplicationIdFromAruid(&appId, aruid));
        ApplicationFileEntry *entries = static_cast<ApplicationFileEntry *>(ptr);
        u64 count = 0;
        {
            std::scoped_lock lk(g_mutex);
            for (u8 i = 0; i < 2; i++) {
                StorageId storage = StorageId(i);
                R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
                R_TRY(ProcessImageDirectory(storage, [&](const Entry &entry) -> bool {
                    if (size == count)
                        return false;

                    if (appId != entry.fileId.applicationId)
                        return true;

                    for (u8 i = 0; i < 4; i++) {
                        if (BIT(type) & BIT(i) && entry.fileId.type == ContentType(i)) {
                            entries[0].entry = *(ApplicationEntry *)&entry;
                            entries[0].datetime = entry.fileId.datetime;
                            entries[0].unk_x28 = 0;
                            entries++;
                            count++;
                            return true;
                        }
                    }
                    return true;
                }));
            }
        }
        R_TRY(control::EncryptAafeBuffer(static_cast<ApplicationFileEntry *>(ptr), count, aruid));
        *outCount = count;
        return ResultSuccess();
    }

    Result GetAlbumFileListAaeAruid(void *ptr, u64 size, u64 *outCount, DateTime start, DateTime end, ContentType type, u64 aruid) {
        *outCount = 0;
        R_UNLESS(type < 4, capsrv::ResultInvalidContentType());
        R_UNLESS(type != ContentType::ExtraScreenshot, capsrv::ResultInvalidContentType());
        u64 appId;
        R_TRY(control::GetApplicationIdFromAruid(&appId, aruid));
        ApplicationEntry *entries = static_cast<ApplicationEntry *>(ptr);
        u64 count = 0;
        {
            std::scoped_lock lk(g_mutex);
            for (u8 i = 0; i < 2; i++) {
                StorageId storage = StorageId(i);
                R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
                R_TRY(ProcessImageDirectory(storage, [&](const Entry &entry) -> bool {
                    if (size == count)
                        return false;

                    if (appId != entry.fileId.applicationId)
                        return true;

                    for (u8 i = 0; i < 4; i++) {
                        if (BIT(type) & BIT(i) && entry.fileId.type == ContentType(i)) {
                            *entries = *(ApplicationEntry *)&entry;
                            entries++;
                            count++;
                            return true;
                        }
                    }
                    return true;
                }));
            }
        }
        R_TRY(control::EncryptAaeBuffer(static_cast<ApplicationEntry *>(ptr), count, aruid));
        *outCount = count;
        return ResultSuccess();
    }

    Result RefreshAlbumCache(StorageId storage) {
        R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
        CapsAlbumCache cache[4]{};
        {
            std::scoped_lock lk(g_mutex);
            R_TRY(MountAlbumImpl(storage));
            R_TRY(ProcessImageDirectory(storage, [&cache](const Entry &entry) -> bool {
                cache[entry.fileId.type].count++;
                cache[entry.fileId.type].unk_x8 += entry.size;
                return true;
            }));
        }
        g_storage.Set(storage, ContentType::Screenshot, cache[ContentType::Screenshot]);
        g_storage.Set(storage, ContentType::Movie, cache[ContentType::Movie]);
        g_storage.Set(storage, ContentType::ExtraScreenshot, cache[ContentType::ExtraScreenshot]);
        g_storage.Set(storage, ContentType::ExtraMovie, cache[ContentType::ExtraMovie]);
        return ResultSuccess();
    }

    Result GetRequiredStorageSpaceSizeToCopyAll(u64 *out, StorageId dst, StorageId src) {
        if (dst == src) {
            *out = 0;
            return ResultSuccess();
        }
        R_UNLESS(config::StorageValid(src), capsrv::ResultInvalidStorageId());
        R_UNLESS(config::StorageValid(dst), capsrv::ResultInvalidStorageId());
        u64 requiredSize = 0;
        {
            std::scoped_lock lk(g_mutex);
            R_TRY(MountAlbumImpl(src));
            R_TRY(MountAlbumImpl(dst));
            R_TRY(ProcessImageDirectory(src, [&requiredSize](const Entry &entry) -> bool {
                requiredSize += entry.size;
                return true;
            }));
        }
        *out = requiredSize;
        return ResultSuccess();
    }

    Result GetAlbumUsageImpl(CapsAlbumContentsUsage *remains, CapsAlbumContentsUsage *usage, u8 usage_size, StorageId storage, u8 flags) {
        R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
        CapsAlbumContentsUsage tmp_remains{};
        CapsAlbumContentsUsage tmp_usage[4]{};
        {
            std::scoped_lock lk(g_mutex);
            R_TRY(ProcessImageDirectory(storage, [&](const Entry &entry) -> bool {
                CapsAlbumContentsUsage &usage = (flags & BIT(entry.fileId.type)) ? tmp_usage[entry.fileId.type] : tmp_remains;
                usage.count++;
                usage.size += entry.size;
                return true;
            }));
        }
        for (u8 i = 0; i < usage_size; i++) {
            usage[i] = {};
            if ((flags & BIT(i)) && (i < 4)) {
                usage[i] = tmp_usage[i];
                usage[i].file_contents = i;
            }
        }
        *remains = tmp_remains;
        remains->flags = 2;
        return ResultSuccess();
    }

    Result GetAlbumUsage(CapsAlbumUsage2 *usage, StorageId storage) {
        return GetAlbumUsageImpl(&usage->usages[1], usage->usages, 1, storage, CapsAlbumFileContentsFlag_ScreenShot);
    }

    Result GetAlbumUsage3(CapsAlbumUsage3 *usage, StorageId storage) {
        return GetAlbumUsageImpl(&usage->usages[2], usage->usages, 2, storage, CapsAlbumFileContentsFlag_ScreenShot | CapsAlbumFileContentsFlag_Movie);
    }

    Result GetAlbumUsage16(CapsAlbumUsage16 *usage, StorageId storage, u8 flags) {
        return GetAlbumUsageImpl(&usage->usages[15], usage->usages, 15, storage, flags);
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

    Result GetAlbumFileSize(u64 *out, const FileId &fileId) {
        std::scoped_lock lk(g_mutex);
        return GetAlbumFileSizeImpl(out, fileId);
    }

    Result SaveAlbumScreenShotFile(const u8 *buffer, u64 size, const FileId &fileId) {
        std::scoped_lock lk(g_mutex);
        return SaveAlbumScreenShotFileImpl(buffer, size, fileId);
    }

    /* Application Accessor. */
    Result DeleteAlbumFileByAruid(ContentType type, const ApplicationEntry &appEntry, u64 aruid) {
        Entry entry;
        R_UNLESS(type == ContentType::ExtraMovie, capsrv::ResultInvalidContentType());
        R_TRY(control::GetAlbumEntryFromApplicationAlbumEntryAruid(&entry, &appEntry, aruid));
        R_UNLESS(type == ContentType::ExtraMovie, capsrv::ResultFileTooBig());

        std::scoped_lock lk(g_mutex);
        return DeleteAlbumFileImpl(entry.fileId);
    }

    Result DeleteAlbumFileByAruidForDebug(const ApplicationEntry &appEntry, u64 aruid) {
        /* TODO: IsDebug? */
        Entry entry;
        R_TRY(control::GetAlbumEntryFromApplicationAlbumEntryAruid(&entry, &appEntry, aruid));

        std::scoped_lock lk(g_mutex);
        return DeleteAlbumFileImpl(entry.fileId);
    }

    Result GetAlbumFileSizeByAruid(u64 *size, const ApplicationEntry &appEntry, u64 aruid) {
        Entry entry;
        R_TRY(control::GetAlbumEntryFromApplicationAlbumEntryAruid(&entry, &appEntry, aruid));
        u64 tmp_size;
        {
            std::scoped_lock lk(g_mutex);
            R_TRY(GetAlbumFileSizeImpl(&tmp_size, entry.fileId));
        }
        *size = tmp_size;
        return ResultSuccess();
    }

    /* TODO: fix. */
    Result PrecheckToCreateContentsByAruid(ContentType type, u64 size) {
        std::scoped_lock lk(g_mutex);
        {
            StorageId storage;
            R_TRY(GetAutoSavingStorageImpl(&storage));
            R_UNLESS(config::GetMax(storage, type) >= g_storage.cache[storage][type].count, capsrv::ResultReachedCountLimit());
            s64 freeSpace;
            R_TRY(fsFsGetFreeSpace(&g_fsFs[storage], "/", &freeSpace));
            R_UNLESS(static_cast<u64>(freeSpace) >= g_storage.cache[storage][type].unk_x8 + size, capsrv::ResultReachedSizeLimit());
        }
        return ResultSuccess();
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

    Result LoadAlbumScreenShotImage(u64 *width, u64 *height, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId) {
        Result rc{};
        Dimensions dims{};
        {
            std::scoped_lock lk(g_mutex);
            rc = LoadScreenShotImage(&dims, nullptr, nullptr, nullptr, img, img_size, work, work_size, fileId);
        }
        if (R_SUCCEEDED(rc)) {
            if (width)
                *width = dims.width;
            if (height)
                *height = dims.height;
        }
        std::memset(work, 0, work_size);
        return rc;
    }

    Result LoadAlbumScreenShotThumbnailImage(u64 *width, u64 *height, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId) {
        Result rc{};
        Dimensions dims{};
        {
            std::scoped_lock lk(g_mutex);
            rc = LoadScreenShotThumbnail(&dims, nullptr, nullptr, nullptr, img, img_size, work, work_size, fileId);
        }
        if (R_SUCCEEDED(rc)) {
            if (width)
                *width = dims.width;
            if (height)
                *height = dims.height;
        }
        std::memset(work, 0, work_size);
        return rc;
    }

    Result LoadAlbumScreenShotImageEx(u64 *width, u64 *height, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        {
            std::scoped_lock lk(g_mutex);
            rc = LoadScreenShotImage(&dims, nullptr, nullptr, nullptr, img, img_size, work, work_size, fileId, opts);
        }
        if (R_SUCCEEDED(rc)) {
            if (width)
                *width = dims.width;
            if (height)
                *height = dims.height;
        }
        std::memset(work, 0, work_size);
        return rc;
    }
    Result LoadAlbumScreenShotThumbnailImageEx(u64 *width, u64 *height, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        {
            std::scoped_lock lk(g_mutex);
            rc = LoadScreenShotThumbnail(&dims, nullptr, nullptr, nullptr, img, img_size, work, work_size, fileId, opts);
        }
        if (R_SUCCEEDED(rc)) {
            if (width)
                *width = dims.width;
            if (height)
                *height = dims.height;
        }
        std::memset(work, 0, work_size);
        return rc;
    }

    Result LoadAlbumScreenShotImageEx0(CapsScreenShotAttribute *out_attr, u64 *width, u64 *height, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        CapsScreenShotAttribute attr{};
        {
            std::scoped_lock lk(g_mutex);
            rc = LoadScreenShotImage(&dims, &attr, nullptr, nullptr, img, img_size, work, work_size, fileId, opts);
        }
        if (R_SUCCEEDED(rc)) {
            if (width)
                *width = dims.width;
            if (height)
                *height = dims.height;
            if (out_attr)
                *out_attr = attr;
        }
        std::memset(work, 0, work_size);
        return rc;
    }
    Result LoadAlbumScreenShotThumbnailImageEx0(CapsScreenShotAttribute *out_attr, u64 *width, u64 *height, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        CapsScreenShotAttribute attr{};
        {
            std::scoped_lock lk(g_mutex);
            rc = LoadScreenShotThumbnail(&dims, &attr, nullptr, nullptr, img, img_size, work, work_size, fileId, opts);
        }
        if (R_SUCCEEDED(rc)) {
            if (width)
                *width = dims.width;
            if (height)
                *height = dims.height;
            if (out_attr)
                *out_attr = attr;
        }
        std::memset(work, 0, work_size);
        return rc;
    }

    Result LoadAlbumScreenShotImageEx1(LoadAlbumScreenShotImageOutput *out, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
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
    Result LoadAlbumScreenShotThumbnailImageEx1(LoadAlbumScreenShotImageOutput *out, void *img, u64 img_size, void *work, u64 work_size, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
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

    Result LoadAlbumScreenShotImageByAruid(LoadAlbumScreenShotImageOutputForApplication *out, void *img, u64 img_size, void *work, u64 work_size, u64 aruid, const ApplicationEntry &appEntry, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        CapsScreenShotAttribute attr{};
        Entry entry{};
        R_TRY(control::GetAlbumEntryFromApplicationAlbumEntryAruid(&entry, &appEntry, aruid));
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
    Result LoadAlbumScreenShotThumbnailImageByAruid(LoadAlbumScreenShotImageOutputForApplication *out, void *img, u64 img_size, void *work, u64 work_size, u64 aruid, const ApplicationEntry &appEntry, const CapsScreenShotDecodeOption &opts) {
        Result rc{};
        Dimensions dims{};
        CapsScreenShotAttribute attr{};
        Entry entry{};
        R_TRY(control::GetAlbumEntryFromApplicationAlbumEntryAruid(&entry, &appEntry, aruid));
        ;
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
