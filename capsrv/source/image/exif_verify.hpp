#pragma once
#include "../capsrv_album_settings.hpp"
#include "../capsrv_types.hpp"
#include "exif_extractor.hpp"

namespace ams::capsrv::image {

    Result VerifyMakerNote(u8 *makernote, const AlbumFileId &file_id, const u8 *jpeg, size_t jpeg_size, AlbumSettings *settings, bool verify_mac);
    Result ExtractMakerNote(size_t *out_makernote_offset, size_t *out_makernote_size, const u8 *jpeg, size_t jpeg_size);
    Result VerifyMAC(const u8 *jpeg, size_t jpeg_size, size_t makernote_offset, size_t makernote_size);

}
