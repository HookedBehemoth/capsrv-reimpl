#pragma once

#include <stratosphere.hpp>

#include "capsrv_movie.hpp"
#include "capsrv_album_settings.hpp"
#include "capsrv_storage.hpp"
#include "capsrv_stream.hpp"
#include "capsrv_types.hpp"

namespace ams::capsrv {

    struct MountStatus {
        MountState state = MountState::unmounted;
        Result last_error = ResultAlbumIsNotMounted();
    };

    class AlbumManager {
      public:
        u8 *work_memory;
        size_t work_memory_size;
        MountStatus nand_mount_status;
        MountStatus sd_mount_status;
        AlbumManagerCache cache;
        AlbumReserve read_reserve;
        AlbumReserve write_reserve;
        AlbumSettings *settings;
        StreamIdGenerator *stream_id_generator;
        MovieController movie_controller;

      public:
        AlbumManager();
        void Initialize(AlbumSettings *set, StreamIdGenerator *gen);
        void Exit();

        void SetMemory(u8 *ptr, size_t size);

        Result MountImageDirectory(StorageId storage_id);
        Result EnsureMountState(StorageId storage_id, MountState mount_state);
        Result CloseImageDirectory(StorageId storage_id);
        Result UnmountImageDirectory(StorageId storage_id);

        Result RefreshCache(StorageId storage_id);
        Result GetCache(CapsAlbumCache *cache, StorageId storage_id, ContentType type);

        Result CanSave(StorageId storage_id, ContentType type, size_t count);
        Result CountEntries(u64 *out_count, StorageId storage_id, u8 content_type_flags);
        Result ListEntries(u64 *out_count, AlbumEntry *entries, size_t entries_count, StorageId storage_id, u8 content_type_flags);
        Result ListApplicationEntries(u64 *out_count, ApplicationAlbumEntry *entries, size_t entries_count, StorageId storage_id, u8 content_type_flags, AlbumDateTime time_span[2], AccountUid acc_uid);

        Result GetFileSize(s64 *out_size, const AlbumFileId &file_id);
        Result LoadImage(size_t *out_size, u8 *buffer, size_t buffer_size, const AlbumFileId &file_id);
        Result LoadJpeg(/* TODO */ size_t *out_size, u8 *buffer, size_t buffer_size, const AlbumFileId &file_id);
        Result LoadThumbnailImage(size_t *out_size, u8 *buffer, size_t buffer_size, const AlbumFileId &file_id);
        Result LoadJpegThumbnail(/* TODO */ size_t *out_size, u8 *buffer, size_t buffer_size, const AlbumFileId &file_id);

        Result DeleteFile(const AlbumFileId &file_id);
        Result CopyFile(const AlbumFileId &src_file_id, StorageId dst_storage_id);
        Result GetAlbumUsage(CapsAlbumContentsUsage *remains, CapsAlbumContentsUsage *usage, u8 count, StorageId storage_id, u8 content_type_flags);
        Result GetRequiredStorageSpaceSizeToCopyAll(size_t *out_size, StorageId src_storage_id, StorageId dst_storage_id);

        Result LoadMakerNoteInfo(size_t *out_size, u8 *maker_note, const AlbumFileId &file_id, void *work_memory, size_t work_memory_size);

        Result SaveScreenshotFile(const u8 *ptr, size_t image_size, const AlbumFileId &file_id, u64 mn_offset = 0, u64 mn_size = 0, u64 mn_unk = 0);
    };

}
