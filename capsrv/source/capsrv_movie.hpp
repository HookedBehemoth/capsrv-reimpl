#pragma once

#include "capsrv_album_settings.hpp"
#include "capsrv_stream.hpp"
#include "capsrv_types.hpp"

namespace ams::capsrv {

    enum class StreamSlotStatus {
        Unused,
    };

    struct TemporaryFile {
        static inline std::atomic<u32> total_count = 0;
        fs::FileHandle tm_file;
        kvdb::BoundedString<0x15> path;
        
        Result Make(s64 file_size);
        void Delete();
        Result Read(s64 offset, u8 *buffer, size_t read_size);
        Result Write(s64 offset, const u8 *buffer, size_t write_size, const fs::WriteOption &option);
        Result SetSize(s64 size);
    };

    enum class StreamStatus {
    };

    class OutputStream {
      private:
        StreamStatus status;
        Result last_error;
        StreamId stream_session;
        StreamId stream_id;
        AlbumFileId file_id;
        fs::FileHandle file_handle;
        TemporaryFile tm_file;
        /* TODO */

      public:
        OutputStream();
        StreamId GetStreamId();
        const AlbumFileId &GetFileId();
        Result GetBrokenReason();

        void CloseByStorage(StorageId storage_id);
        Result OpenStream();
    };                    // 0xD8
    class InputStream {}; // 0x580

    class MovieController {
      private:
        AlbumSettings *settings;
        StreamIdGenerator *stream_id_generator;
        AlbumReserve *read_reserve;
        AlbumReserve *write_reserve;
        StreamSlotStatus output_stream_status[4];
        OutputStream output_streams[4];
        StreamSlotStatus input_stream_status[2];
        OutputStream input_streams[2];

      public:
        MovieController();
        void Initialize(AlbumSettings *set, StreamIdGenerator *gen, AlbumReserve *read_reserve, AlbumReserve *write_reserve);
        void Exit();
        void CloseStreamsByStorageId(StorageId storage_id);
        Result OpenReadStream(StreamId *out, const AlbumFileId &file_id, u64 session, u8*);
    };

}
