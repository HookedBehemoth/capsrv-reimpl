#pragma once
#include "../capsrv_types.hpp"
#include "exif_extractor.hpp"

namespace ams::capsrv::image {

    Result VerifyMakerNote(u8 *makerNote, const u8 *jpegBuffer, size_t bufferSize, const FileId &fileId, bool verifyMac);
    Result ExtractMakerNote(size_t *makerNoteOffsetOut, size_t *makerNoteSize, const u8 *jpegBuffer, size_t bufferSize);
    Result VerifyMAC(const u8 *jpegBuffer, size_t jpegSize, size_t makerNoteOffset, size_t makerNoteSize);

}
