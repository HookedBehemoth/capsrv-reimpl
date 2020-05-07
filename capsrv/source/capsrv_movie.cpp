#include "capsrv_movie.hpp"

#include "capsrv_environment.hpp"

namespace ams::capsrv {

    Result TemporaryFile::Make(s64 file_size) {
        /* Quests units aren't allowed to record videos. N caches this. */
        if (IsQuest())
            return capsrv::ResultAlbumIsFull();

        /* Increase TemporaryFileCount by one. */
        this->total_count++;
        auto count_guard = SCOPE_GUARD { --this->total_count; };
        R_UNLESS(this->total_count < 0x20, capsrv::ResultInternalAlbumTemporaryFileCountLimit());

        /* Try creating a file 0x100 times. */
        for (int i = 0; i < 0x100; i++) {
            /* Random file name. */
            char path_buffer[0x15];
            u64 rand_id;
            os::GenerateRandomBytes(&rand_id, sizeof(u64));
            std::snprintf(path_buffer, 0x15, "TM:/%016lX", rand_id);

            /* If creating a file failed because the file already exists, retry. */
            R_TRY_CATCH(fs::CreateFile(path_buffer, file_size)) {
                R_CATCH(fs::ResultPathAlreadyExists)
                    continue;
                R_CONVERT_ALL(capsrv::ResultInternalAlbumTemporaryFileCreateError())
            }
            R_END_TRY_CATCH;

            /* Try to open the file. */
            fs::FileHandle file_handle;
            if (R_FAILED(fs::OpenFile(&file_handle, path_buffer, fs::OpenMode_All))) {
                fs::DeleteFile(path_buffer);
                break;
            }

            /* Copy path and filehandle. */
            count_guard.Cancel();
            this->tm_file = file_handle;
            this->path.Set(path_buffer);
            return ResultSuccess();
        }

        return capsrv::ResultInternalAlbumTemporaryFileCreateRetryCountLimit();
    }

    void TemporaryFile::Delete() {
        fs::FlushFile(this->tm_file);
        fs::CloseFile(this->tm_file);
        fs::DeleteFile(this->path);

        u32 count = this->total_count;
        --this->total_count;

        AMS_ABORT_UNLESS(count > 0);
    }

    Result TemporaryFile::Read(s64 offset, u8 *buffer, size_t read_size) {
        if (R_FAILED(fs::ReadFile(this->tm_file, offset, buffer, read_size))) {
            return capsrv::ResultInternalAlbumTemporaryFileReadFileError();
        } else {
            return ResultSuccess();
        }
    }

    Result TemporaryFile::Write(s64 offset, const u8 *buffer, size_t write_size, const fs::WriteOption &option) {
        if (R_FAILED(fs::WriteFile(this->tm_file, offset, buffer, write_size, option))) {
            return capsrv::ResultInternalAlbumTemporaryFileWriteFileError();
        } else {
            return ResultSuccess();
        }
    }

    Result TemporaryFile::SetSize(s64 size) {
        if (R_FAILED(fs::SetFileSize(this->tm_file, size))) {
            return capsrv::ResultInternalAlbumTemporaryFileSetFileSizeError();
        } else {
            return ResultSuccess();
        }
    }
    OutputStream::OutputStream() {
    }

    StreamId OutputStream::GetStreamId() {
        return this->stream_id;
    }

    const AlbumFileId &OutputStream::GetFileId() {
        return this->file_id;
    }

    Result OutputStream::GetBrokenReason() {
        return this->last_error;
    }

    void OutputStream::CloseByStorage(StorageId storage_id) {
        AMS_ABORT();
    }

    Result OutputStream::OpenStream() {
        AMS_ABORT();
    }

    MovieController::MovieController()
        : settings(), stream_id_generator(), output_streams({}), input_streams({}) {
    }

    void MovieController::Initialize(AlbumSettings *set, StreamIdGenerator *gen, AlbumReserve *read_reserve, AlbumReserve *write_reserve) {
        std::memset(&this->output_stream_status, 0, 0); /* TODO: memset correct size */
        this->settings = set;
        this->stream_id_generator = gen;
        this->read_reserve = read_reserve;
        this->write_reserve = write_reserve;
    }

    void MovieController::Exit() {
        /* Official software memsets itself on exit. */
    }

    void MovieController::CloseStreamsByStorageId(StorageId storage_id) {
        for (int i = 0; i < 4; i++)
            if (this->output_stream_status[i] != StreamSlotStatus::Unused)
                this->output_streams[i].CloseByStorage(storage_id);

        for (int i = 0; i < 2; i++)
            if (this->input_stream_status[i] != StreamSlotStatus::Unused)
                this->input_streams[i].CloseByStorage(storage_id);
    }

}
