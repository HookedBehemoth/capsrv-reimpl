#pragma once

#include <stratosphere.hpp>

#include "capsrv_types.hpp"

namespace ams::capsrv {

    constexpr const size_t OverlayHeight = 54;
    constexpr const size_t OverlayWidth = 96;
    constexpr const size_t OverlayComponents = 4;

    constexpr const size_t OverlayBufferSize = OverlayHeight * OverlayWidth * OverlayComponents;

    enum class OverlayThumbnailStatus {
        NotInitialized,
        Initialized,
    };

    enum class OverlayStatus {
        Free,
        Occupied,
    };

    struct OverlayCacheData {
        u8 *buffer;
        AlbumFileId file_id;
        OverlayStatus status;
    };

    class OverlayThumbnailHolder {
      private:
        OverlayThumbnailStatus status;
        u8 *total_memory;
        size_t memory_size;

        OverlayCacheData data[2];

        os::Mutex mutex;
        os::ConditionVariable cond_var;

      public:
        OverlayThumbnailHolder();

        size_t GetBufferSize();
        void SetMemory(u8 *buffer, size_t buffer_size);
        void Initialize();
        void Exit();

        Result GetLastOverlayThumbnail(AlbumFileId *out_file_id, size_t *out_size, u8 *out_buffer, size_t out_buffer_size, ContentType type);
        u8 *GetOverlayBuffer(const AlbumFileId &file_id, ContentType type);

        void Signal(ContentType type);
    };

}
