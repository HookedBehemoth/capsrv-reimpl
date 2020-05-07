#include "capsrv_album_manager.hpp"
#include "capsrv_storage.hpp"
#include "capsrv_environment.hpp"

namespace ams::capsrv {

    namespace {

        Result CreateAlbumFileImpl(fs::FileHandle *file, char *path, s64 size, int mode) {
            R_UNLESS(!IsQuest(), capsrv::ResultAlbumIsFull());

            /* Ensure the path to the file exists. */
            R_TRY(fs::EnsureParentDirectoryRecursively(path));
            
            /* Create the file. */
            R_TRY_CATCH(fs::CreateFile(path, size, fs::CreateOption_None)) {
                R_CATCH(fs::ResultPathAlreadyExists) {
                    /* Open existing file. */
                    R_TRY(fs::OpenFile(file, path, mode));
                    auto close_guard = SCOPE_GUARD { fs::CloseFile(*file); };

                    /* Overwrite file size. */
                    R_TRY(fs::SetFileSize(*file, size));

                    /* Cleanup */
                    close_guard.Cancel();
                    return ResultSuccess();
                }
            } R_END_TRY_CATCH;

            /* Delete new file on failure. */
            auto delete_guard = SCOPE_GUARD { fs::DeleteFile(path); };

            /* Open file. */
            R_TRY(fs::OpenFile(file, path, mode));

            /* Cleanup */
            delete_guard.Cancel();
            return ResultSuccess();
        }

    }

    AlbumManager::AlbumManager()
        : work_memory(), work_memory_size(), nand_mount_status(), sd_mount_status(), cache(), read_reserve(), write_reserve(), settings(), stream_id_generator(), movie_controller() {
    }

    void AlbumManager::Initialize(AlbumSettings *set, StreamIdGenerator *gen) {
        this->settings = set;
        this->stream_id_generator = gen;

        if (hosversionAtLeast(4, 0, 0)) {
            /* Attempt to open savedata filesystem and mount it. */
            Result rc = fs::CreateSystemSaveData(0x8000000000000140, 0x80000, 0x80000, 0);
            if (R_SUCCEEDED(rc) || (rc.GetValue() == fs::ResultPathAlreadyExists().GetValue())) {
                R_ABORT_UNLESS(fs::MountSystemSaveData("TM", 0x8000000000000140));
            } else {
                AMS_ABORT();
            }
        }
        nand_mount_status = MountStatus();
        sd_mount_status = MountStatus();
        this->read_reserve = AlbumReserve();
        this->write_reserve = AlbumReserve();
        this->movie_controller.Initialize(set, stream_id_generator, &this->read_reserve, &this->write_reserve);
    }

    void AlbumManager::Exit() {
        this->movie_controller.CloseStreamsByStorageId(StorageId::Nand);
        if (this->nand_mount_status.state == MountState::mounted) {
            UnmountAlbum(StorageId::Nand, this->settings);
        }
        this->nand_mount_status = MountStatus();

        this->movie_controller.CloseStreamsByStorageId(StorageId::Sd);
        if (this->sd_mount_status.state == MountState::mounted) {
            UnmountAlbum(StorageId::Sd, this->settings);
        }
        this->sd_mount_status = MountStatus();

        fs::Unmount("TM");

        this->movie_controller.Exit();
        this->settings = nullptr;
        this->stream_id_generator = nullptr;
    }

    void AlbumManager::SetMemory(u8 *ptr, size_t size) {
        this->work_memory = ptr;
        this->work_memory_size = size;
    }

    Result AlbumManager::MountImageDirectory(StorageId storage_id) {
        R_TRY(VerifyStorageId(storage_id));

        /* Obtain mount status of the requested storage. */
        MountStatus &mount_status = (storage_id == StorageId::Nand) ? this->nand_mount_status : this->sd_mount_status;

        /* If storage isn't available return the last result code. */
        R_UNLESS(mount_status.state == MountState::available, mount_status.last_error);

        Result last_result;

        R_TRY_CATCH(MountAlbum(storage_id, this->settings)) {
            R_CATCH(capsrv::ResultAlbumPathUnavailable) {
                mount_status.state = MountState::unmounted;
                last_result = capsrv::ResultAlbumPathNotFound();
            }
            R_CATCH(capsrv::ResultAlbumUnexpected) {
                mount_status.state = MountState::unmounted;
                last_result = capsrv::ResultAlbumIsNotMounted();
            }
            R_CATCH_ALL() {
                mount_status.state = MountState::available;
                last_result = capsrv::ResultAlbumIsNotMounted();
            }
            } else {}
        } else {
            mount_status.state = MountState::mounted;
            last_result = ResultSuccess();
        }});

        mount_status.last_error = last_result;
        return last_result;
    }

    Result AlbumManager::EnsureMountState(StorageId storage_id, MountState mount_state) {
        R_TRY(VerifyStorageId(storage_id));

        /* Obtain mount status of the requested storage. */
        MountStatus &mount_status = (storage_id == StorageId::Nand) ? this->nand_mount_status : this->sd_mount_status;
        
        if (mount_status.state != MountState::available) {
            Result last_result = mount_status.last_error;
            R_SUCCEED_IF(last_result.IsSuccess());
            
            if (last_result.GetValue() != 206) {
                return last_result;
            } else if (capsrv::ResultAlbumPathUnavailable().Includes(last_result)) {
                return last_result;
            }

            Result aaaaaa = capsrv::ResultAlbumUnknown73();
            if (mount_state != MountState::mounted)
                aaaaaa = capsrv::ResultAlbumPathUnavailable();

            Result aaaaaaa = capsrv::ResultAlbumUnknown72();
            if (mount_state != MountState::available)
                aaaaaaa = aaaaaa;

            return aaaaaa;
        }

        Result last_result;

        R_TRY_CATCH(MountAlbum(storage_id, this->settings)) {
            R_CATCH(capsrv::ResultAlbumPathUnavailable) {
                mount_status.state = MountState::unmounted;
                last_result = capsrv::ResultAlbumPathNotFound();
            }
            R_CATCH(capsrv::ResultAlbumUnexpected) {
                mount_status.state = MountState::unmounted;
                last_result = capsrv::ResultAlbumIsNotMounted();
            }
            R_CATCH_ALL() {
                mount_status.state = MountState::available;
                last_result = capsrv::ResultAlbumIsNotMounted();
            }
            } else {}
        } else {
            mount_status.state = MountState::mounted;
            last_result = ResultSuccess();
        }});

        mount_status.last_error = last_result;
        return last_result;
    }

    Result AlbumManager::CloseImageDirectory(StorageId storage_id) {
        R_TRY(VerifyStorageId(storage_id));

        /* Obtain mount status of the requested storage. */
        MountStatus &mount_status = (storage_id == StorageId::Nand) ? this->nand_mount_status : this->sd_mount_status;

        if (mount_status.state == MountState::mounted) {
            this->movie_controller.CloseStreamsByStorageId(storage_id);
            R_TRY(UnmountAlbum(storage_id, this->settings));
        }

        /* Update mount status. */
        mount_status.state = MountState::unmounted;
        mount_status.last_error = capsrv::ResultAlbumIsNotMounted();
        return ResultSuccess();
    }

    Result AlbumManager::UnmountImageDirectory(StorageId storage_id) {
        R_TRY(VerifyStorageId(storage_id));

        /* Obtain mount status of the requested storage. */
        MountStatus &mount_status = (storage_id == StorageId::Nand) ? this->nand_mount_status : this->sd_mount_status;

        if (mount_status.state == MountState::mounted) {
            this->movie_controller.CloseStreamsByStorageId(storage_id);
            R_TRY(UnmountAlbum(storage_id, this->settings));
        }

        /* Update mount status. */
        mount_status.state = MountState::available;
        mount_status.last_error = capsrv::ResultAlbumIsNotMounted();
        return ResultSuccess();
    }

    Result AlbumManager::RefreshCache(StorageId storage_id) {
        std::memset(this->work_memory, 0, this->work_memory_size);

        /* Cleanup on exit. */
        ON_SCOPE_EXIT { std::memset(this->work_memory, 0, this->work_memory_size); };

        R_TRY(VerifyStorageId(storage_id));
        R_TRY(this->EnsureMountState(storage_id, MountState::mounted));

        /* TODO: Process image directory */
        CapsAlbumCache cache[4];

        /* Updated cache. */
        this->cache.Set(storage_id, ContentType::Screenshot, cache[0]);
        this->cache.Set(storage_id, ContentType::Movie, cache[1]);
        this->cache.Set(storage_id, ContentType::ExtraScreenshot, cache[2]);
        this->cache.Set(storage_id, ContentType::ExtraMovie, cache[3]);
        return ResultSuccess();
    }

    Result AlbumManager::GetCache(CapsAlbumCache *cache, StorageId storage_id, ContentType type) {
        *cache = {};

        R_TRY(VerifyStorageId(storage_id));
        R_TRY(VerifyContentType(type, this->settings));

        *cache = this->cache.Get(storage_id, type);

        return ResultSuccess();
    }

    Result AlbumManager::CanSave(StorageId storage_id, ContentType type, size_t count) {
        R_TRY(VerifyStorageId(storage_id));
        R_TRY(VerifyContentType(type, this->settings));
        
        size_t max_count = this->settings->GetFileMaxCount(storage_id, type);
        size_t current_count = this->cache.GetCount(storage_id, type);
        R_UNLESS(current_count < max_count, capsrv::ResultInternalAlbumLimitationFileCountLimit());
        return ResultSuccess();
    }

    Result AlbumManager::CountEntries(u64 *out_count, StorageId storage_id, u8 content_type_flags) {
        std::memset(this->work_memory, 0, this->work_memory_size);
        ON_SCOPE_EXIT { std::memset(this->work_memory, 0, this->work_memory_size); };

        R_TRY(VerifyStorageId(storage_id));
        R_TRY(this->EnsureMountState(storage_id, MountState::mounted));

        /* TODO: Process image directory */
        size_t count = 0;

        /* Updated cache. */
        *out_count = count;
        return ResultSuccess();
    }

    Result AlbumManager::ListEntries(u64 *out_count, AlbumEntry *entries, size_t entries_count, StorageId storage_id, u8 content_type_flags) {
        std::memset(this->work_memory, 0, this->work_memory_size);
        ON_SCOPE_EXIT { std::memset(this->work_memory, 0, this->work_memory_size); };

        *out_count = 0;
        std::memset(entries, 0, entries_count * sizeof(AlbumEntry));

        R_TRY(this->EnsureMountState(storage_id, MountState::mounted));

        /* TODO: Process image directory */
        size_t count = 0;

        /* Updated cache. */
        *out_count = count;
        return ResultSuccess();
    }

    Result AlbumManager::ListApplicationEntries(u64 *out_count, ApplicationAlbumEntry *entries, size_t entries_count, StorageId storage_id, u8 content_type_flags, AlbumDateTime time_span[2], AccountUid acc_uid) {
        std::memset(this->work_memory, 0, this->work_memory_size);
        ON_SCOPE_EXIT { std::memset(this->work_memory, 0, this->work_memory_size); };

        *out_count = 0;
        std::memset(entries, 0, entries_count * sizeof(ApplicationAlbumEntry));

        R_TRY(this->EnsureMountState(storage_id, MountState::mounted));

        /* TODO: Process image directory */
        size_t count = 0;

        /* Updated cache. */
        *out_count = count;
        return ResultSuccess();
    }

    Result AlbumManager::GetFileSize(s64 *out_size, const AlbumFileId &file_id) {
        *out_size = 0;

        /* File write isn't completed. */
        R_UNLESS(!this->write_reserve.IsReserved(file_id), capsrv::ResultAlbumFileNotFound());
        R_TRY(file_id.Verify(this->settings));
        R_TRY(this->EnsureMountState(file_id.storage_id, MountState::mounted));

        /* Make path. */
        AlbumPath path;
        R_TRY(path.MakeAlbumPath(file_id, 1, this->settings));

        /* Open file. */
        fs::FileHandle file_handle;
        R_TRY(fs::OpenFile(&file_handle, path.path_buffer, fs::OpenMode_Read));
        ON_SCOPE_EXIT { fs::CloseFile(file_handle); };
        
        /* Get file size. */
        s64 file_size;
        R_TRY(fs::GetFileSize(&file_size, file_handle));

        s64 max_size = GetAllowedFileSize(file_id.type, settings);
        R_UNLESS(file_size <= max_size, capsrv::ResultAlbumFileNotFound());

        *out_size = static_cast<u64>(file_size);

        return ResultSuccess();
    }

    Result AlbumManager::LoadImage(size_t *out_size, u8 *buffer, size_t buffer_size, const AlbumFileId &file_id) {
        return this->LoadJpeg(out_size, buffer, buffer_size, file_id);
    }

    Result AlbumManager::LoadJpeg(/* TODO */ size_t *out_size, u8 *buffer, size_t buffer_size, const AlbumFileId &file_id) {
        std::memset(this->work_memory, 0, this->work_memory_size);
        ON_SCOPE_EXIT { std::memset(this->work_memory, 0, this->work_memory_size); };

        *out_size = 0;
        std::memset(buffer, 0, buffer_size);
        auto jpeg_guard = SCOPE_GUARD { std::memset(buffer, 0, buffer_size); };

        R_UNLESS(!this->write_reserve.IsReserved(file_id), capsrv::ResultAlbumFileNotFound());
        R_TRY(file_id.Verify(this->settings));
        R_TRY(this->EnsureMountState(file_id.storage_id, MountState::mounted));

        R_UNLESS(this->settings->IsTypeSupported(file_id.type), capsrv::ResultAlbumInvalidFileContents());

        if (IsMovie(file_id.type)) {
            /* Load jpeg from movie. */
        } else {
            /* Load jpeg from image. */
        }

        jpeg_guard.Cancel();
        return ResultSuccess();
    }

    Result AlbumManager::LoadThumbnailImage(size_t *out_size, u8 *buffer, size_t buffer_size, const AlbumFileId &file_id) {
        return LoadJpegThumbnail(out_size, buffer, buffer_size, file_id);
    }

    Result AlbumManager::LoadJpegThumbnail(/* TODO */ size_t *out_size, u8 *buffer, size_t buffer_size, const AlbumFileId &file_id) {
        std::memset(this->work_memory, 0, this->work_memory_size);
        ON_SCOPE_EXIT { std::memset(this->work_memory, 0, this->work_memory_size); };

        *out_size = 0;
        std::memset(buffer, 0, buffer_size);
        auto jpeg_guard = SCOPE_GUARD { std::memset(buffer, 0, buffer_size); };

        R_UNLESS(!this->write_reserve.IsReserved(file_id), capsrv::ResultAlbumFileNotFound());
        R_TRY(file_id.Verify(this->settings));
        R_TRY(this->EnsureMountState(file_id.storage_id, MountState::mounted));

        R_UNLESS(this->settings->IsTypeSupported(file_id.type), capsrv::ResultAlbumInvalidFileContents());

        if (IsMovie(file_id.type)) {
            /* Load jpeg thumbnail from movie. */
        } else {
            /* Load jpeg thumbnail from image. */
        }

        jpeg_guard.Cancel();
        return ResultSuccess();
    }

    Result AlbumManager::DeleteFile(const AlbumFileId &file_id) {
        R_UNLESS(!this->read_reserve.IsReserved(file_id), capsrv::ResultAlbumDestinationAccessCorrupted()); /// ???
        R_UNLESS(!this->write_reserve.IsReserved(file_id), capsrv::ResultAlbumFileNotFound());
        R_TRY(file_id.Verify(this->settings));
        R_TRY(this->EnsureMountState(file_id.storage_id, MountState::mounted));

        AlbumPath file_path;
        R_TRY(file_path.MakeAlbumPath(file_id, 2, this->settings));

        /* Official software attempts to delete empty folders. */
        fs::DeleteFile(file_path.path_buffer);

        this->cache.Decrement(file_id.storage_id, file_id.type);

        return ResultSuccess();
    }

    Result AlbumManager::CopyFile(const AlbumFileId &src_file_id, StorageId dst_storage_id) {
        std::memset(this->work_memory, 0, this->work_memory_size);
        ON_SCOPE_EXIT { std::memset(this->work_memory, 0, this->work_memory_size); };

        R_UNLESS(!this->write_reserve.IsReserved(src_file_id), capsrv::ResultAlbumFileNotFound());
        R_TRY(src_file_id.Verify(this->settings));
        R_TRY(VerifyStorageId(dst_storage_id));
        R_SUCCEED_IF(dst_storage_id == src_file_id.storage_id);

        /* Ensure storage is mounted. */
        R_TRY(this->EnsureMountState(src_file_id.storage_id, MountState::mounted));
        R_TRY(this->EnsureMountState(dst_storage_id, MountState::mounted));

        AlbumFileId dst_file_id = src_file_id;
        dst_file_id.storage_id = dst_storage_id;

        /* Make path to file. */
        AlbumPath src_path, dst_path;
        R_TRY(src_path.MakeAlbumPath(src_file_id, 1, this->settings));
        R_TRY(dst_path.MakeAlbumPath(dst_file_id, 2, this->settings));

        /* Open source file. */
        fs::FileHandle src_file_handle;
        R_TRY(fs::OpenFile(&src_file_handle, src_path.path_buffer, fs::OpenMode_Read));
        ON_SCOPE_EXIT { fs::CloseFile(src_file_handle); };
        
        /* Get file size. */
        s64 file_size;
        R_TRY(fs::GetFileSize(&file_size, src_file_handle));

        size_t max_size = GetAllowedFileSize(src_file_id.type, this->settings);
        R_UNLESS(file_size <= static_cast<s64>(max_size), capsrv::ResultAlbumFileNotFound());

        /* Ensure we can save the file. */
        ContentType type = src_file_id.type;
        size_t max_count = this->settings->GetFileMaxCount(dst_storage_id, type);
        size_t current_count = this->cache.GetCount(dst_storage_id, type);
        R_UNLESS(current_count < max_count, ResultInternalAlbumLimitationFileCountLimit());

        /* Open destination file. */
        fs::FileHandle dst_file_handle;
        R_TRY(CreateAlbumFileImpl(&dst_file_handle, dst_path.path_buffer, file_size, fs::OpenMode_Write | fs::OpenMode_AllowAppend));
        auto file_failure_guard = SCOPE_GUARD { fs::DeleteFile(dst_path.path_buffer); };
        ON_SCOPE_EXIT { fs::CloseFile(dst_file_handle); };

        /* Read file to cache and write to new file. */
        s64 offset = 0;
        u64 read_size;
        while (offset < file_size) {
            R_TRY(fs::ReadFile(&read_size, src_file_handle, offset, this->work_memory, this->work_memory_size));
            R_TRY(fs::WriteFile(dst_file_handle, offset, this->work_memory, read_size, fs::WriteOption::None));
            offset += read_size;
        }

        /* Cleanup. */
        fs::FlushFile(dst_file_handle);
        file_failure_guard.Cancel();
        this->cache.Increment(dst_storage_id, type);
        return ResultSuccess();
    }

    Result AlbumManager::GetAlbumUsage(CapsAlbumContentsUsage *remains, CapsAlbumContentsUsage *usage, u8 count, StorageId storage_id, u8 content_type_flags) {
        std::memset(this->work_memory, 0, this->work_memory_size);
        ON_SCOPE_EXIT { std::memset(this->work_memory, 0, this->work_memory_size); };

        *remains = {};
        std::memset(usage, 0, count * sizeof(CapsAlbumContentsUsage));
        auto cleanup_guard = SCOPE_GUARD { *remains = {}; std::memset(usage, 0, count * sizeof(CapsAlbumContentsUsage)); };

        R_TRY(this->EnsureMountState(storage_id, MountState::mounted));

        /* TODO: Process image directory */

        cleanup_guard.Cancel();
        return ResultSuccess();
    }

    Result AlbumManager::GetRequiredStorageSpaceSizeToCopyAll(size_t *out_size, StorageId src_storage_id, StorageId dst_storage_id) {
        std::memset(this->work_memory, 0, this->work_memory_size);
        ON_SCOPE_EXIT { std::memset(this->work_memory, 0, this->work_memory_size); };

        *out_size = 0;

        /* Ensure storage is mounted. */
        R_TRY(this->EnsureMountState(src_storage_id, MountState::mounted));
        R_TRY(this->EnsureMountState(dst_storage_id, MountState::mounted));
        R_SUCCEED_IF(src_storage_id == dst_storage_id);

        /* TODO: Process image directory */
        size_t required_size = 0;

        *out_size = required_size;
        return ResultSuccess();
    }

    Result AlbumManager::LoadMakerNoteInfo(size_t *out_size, u8 *maker_note, const AlbumFileId &file_id, void *work_memory, size_t work_memory_size) {
        *out_size = 0;

        /* Only available on [6.0.0]+ so we only care for it's buffer size */
        //size_t makernote_buffer_size = 0xc78;
        //u8 tmp_makernote[makernote_buffer_size];

        return ResultSuccess();
    }

    Result AlbumManager::SaveScreenshotFile(const u8 *ptr, size_t image_size, const AlbumFileId &file_id, u64 mn_offset, u64 mn_size, u64 mn_unk) {
        return ResultSuccess();
    }

}
