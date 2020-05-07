#include "capsrv_overlay_holder.hpp"

#include "capsrv_types.hpp"

namespace ams::capsrv {

    OverlayThumbnailHolder::OverlayThumbnailHolder() : mutex(false) {
        this->status = OverlayThumbnailStatus::NotInitialized;
        this->total_memory = nullptr;
        this->memory_size = 0;
    }

    size_t OverlayThumbnailHolder::GetBufferSize() {
        return OverlayBufferSize;
    }
    void OverlayThumbnailHolder::SetMemory(u8 *buffer, size_t buffer_size) {
        this->total_memory = buffer;
        this->memory_size = buffer_size;
    }

    void OverlayThumbnailHolder::Initialize() {
        u8 *ptr = this->total_memory;
        this->data[0] = {ptr, {}, OverlayStatus::Free};
        std::memset(ptr, 0xFF, OverlayBufferSize);

        ptr += OverlayBufferSize;
        this->data[1] = {ptr, {}, OverlayStatus::Free};
        std::memset(ptr, 0xFF, OverlayBufferSize);

        this->status = OverlayThumbnailStatus::Initialized;
    }

    void OverlayThumbnailHolder::Exit() {
        this->status = OverlayThumbnailStatus::NotInitialized;
    }

    Result OverlayThumbnailHolder::GetLastOverlayThumbnail(AlbumFileId *out_file_id, size_t *out_size, u8 *out_buffer, size_t out_buffer_size, ContentType type) {
        R_UNLESS(out_buffer_size >= OverlayBufferSize, capsrv::ResultAlbumReadBufferShortage());

        /* Acquire mutex. */
        std::scoped_lock lk(this->mutex);
        OverlayStatus *status = &this->data[static_cast<u8>(type)].status;

        /* Wait for occupation status to change. */
        while (*status == OverlayStatus::Occupied) {
            cond_var.Wait(this->mutex);
        }

        /* Copy data. */
        const OverlayCacheData &data = this->data[static_cast<u8>(type)];
        std::memcpy(out_buffer, data.buffer, OverlayBufferSize);
        *out_size = OverlayBufferSize;
        *out_file_id = data.file_id;

        return ResultSuccess();
    }

    u8 *OverlayThumbnailHolder::GetOverlayBuffer(const AlbumFileId &file_id, ContentType type) {
        /* Acquire mutex. */
        std::scoped_lock lk(this->mutex);
        OverlayStatus *status = &this->data[static_cast<u8>(type)].status;

        /* Wait for occupation status to change. */
        while (*status == OverlayStatus::Occupied) {
            cond_var.Wait(this->mutex);
        }

        /* Store AlbumFileId, return pointer to buffer. */
        OverlayCacheData &data = this->data[static_cast<u8>(type)];
        data.file_id = file_id;
        return data.buffer;
    }

    void OverlayThumbnailHolder::Signal(ContentType type) {
        /* Acquire mutex. */
        std::scoped_lock lk(this->mutex);
        this->data[static_cast<u8>(type)].status = OverlayStatus::Free;
        this->cond_var.Signal();
    }

}
