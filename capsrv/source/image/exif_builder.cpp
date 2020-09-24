#include "exif_builder.hpp"

namespace ams::image {

    namespace {

        struct ExifBuilderInternal {
            u8 filler[0x2a0];
            const char *software        = nullptr;
            u16 software_size           = 0;
            ExifOrientation orientation = ExifOrientation::Invalid;
            const char *date_time       = nullptr;
            u16 date_time_size          = 0;
            const void *maker_note      = nullptr;
            u16 maker_note_size         = 0;
            const char *unique_id       = nullptr;
            const void *thumbnail       = nullptr;
            size_t thumbnail_size       = 0;
        };

        static_assert(sizeof(ExifBuilderInternal) == 0x2e8);

    }

    size_t ExifBuilder::GetWorkBufferSize() {
        return sizeof(ExifBuilderInternal);
    }

    ExifBuilder::ExifBuilder(void *work_buffer, size_t work_buffer_size) : stage(ProcessStage_Registered), work_buffer(work_buffer) {
        new (work_buffer) ExifBuilderInternal();
    }

    void ExifBuilder::SetOrientation(ExifOrientation orientation) {
        auto internal = static_cast<ExifBuilderInternal *>(this->work_buffer);

        internal->orientation = orientation;

        this->stage = ProcessStage_Registered;
    }

    void ExifBuilder::SetSoftware(const char *software, size_t length) {
        auto internal = static_cast<ExifBuilderInternal *>(this->work_buffer);

        internal->software      = software;
        internal->software_size = software == nullptr ? 0 : length;

        this->stage = ProcessStage_Registered;
    }

    void ExifBuilder::SetDateTime(const char *date_time, size_t length) {
        auto internal = static_cast<ExifBuilderInternal *>(this->work_buffer);

        internal->date_time = date_time;

        this->stage = ProcessStage_Registered;
    }

    void ExifBuilder::SetMakerNote(const void *maker_note, size_t size) {
        auto internal = static_cast<ExifBuilderInternal *>(this->work_buffer);

        internal->maker_note      = maker_note;
        internal->maker_note_size = maker_note == nullptr ? 0 : size;

        this->stage = ProcessStage_Registered;
    }

    void ExifBuilder::SetUniqueId(const char *unique_id, size_t length) {
        auto internal = static_cast<ExifBuilderInternal *>(this->work_buffer);

        internal->unique_id = unique_id;

        this->stage = ProcessStage_Registered;
    }

    void ExifBuilder::SetThumbnail(const void *thumbnail, size_t size) {
        auto internal = static_cast<ExifBuilderInternal *>(this->work_buffer);

        internal->thumbnail      = thumbnail;
        internal->thumbnail_size = thumbnail == nullptr ? 0 : size;

        this->stage = ProcessStage_Registered;
    }

}
