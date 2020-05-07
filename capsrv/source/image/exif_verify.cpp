#include "exif_verify.hpp"

#include "../capsrv_crypto.hpp"

namespace ams::capsrv::image {

    Result VerifyMakerNote(u8 *out_makernote, const AlbumFileId &file_id, const u8 *jpeg, size_t jpeg_size, AlbumSettings *settings, bool verify_mac) {
        bool verify = settings->GetVerifyScreenshotFiledata();
        R_UNLESS(settings->IsTypeSupported(file_id.type), capsrv::ResultAlbumInvalidFileContents());
        R_UNLESS(jpeg_size != 0, capsrv::ResultInternalFileDataVerificationEmptyFileData());

        if (verify_mac && verify) {
            size_t makernote_offset;
            size_t makernote_size;
            R_TRY(ExtractMakerNote(&makernote_offset, &makernote_size, jpeg, jpeg_size));
            R_TRY(VerifyMAC(jpeg, jpeg_size, makernote_offset, makernote_size));
        }

        std::memset(out_makernote, 0, 0xc90);

        const u8 *exif_region = nullptr;
        u32 exif_size = 0;
        s32 res = ams::image::detail::ExtractExifRegion(&exif_region, &exif_size, jpeg, jpeg_size);
        R_UNLESS(res == 0,                  capsrv::ResultInternalFileDataVerificationExifExtractionFailed());
        R_UNLESS(exif_region != nullptr,    capsrv::ResultInternalFileDataVerificationExifExtractionFailed());
        R_UNLESS(exif_size != 0,            capsrv::ResultInternalFileDataVerificationExifExtractionFailed());

        ams::image::detail::ExifBinary exifBinary;
        ams::image::ExifExtractor exifExtractor(&exifBinary);
        exifExtractor.SetExifData(exif_region, exif_size);
        R_UNLESS(exifExtractor.Analyse(), capsrv::ResultInternalFileDataVerificationExifAnalyzationFailed());
        
        u32 dateStringLength;
        const char *dateString = exifExtractor.ExtractDateTime(&dateStringLength);
        R_UNLESS((dateString && dateStringLength) || !verify, capsrv::ResultInternalFileDataVerificationDateTimeExtractionFailed());

        ams::image::ExifOrientation orientation;
        bool success = exifExtractor.ExtractOrientation(&orientation);
        if (!success)
            orientation = ams::image::ExifOrientation::UpperLeft;

        u32 makernote_size;
        const u8 *makerNote = exifExtractor.ExtractMakerNote(&makernote_size);
        R_UNLESS((makerNote && makernote_size) || !verify, capsrv::ResultInternalFileDataVerificationMakerNoteExtractionFailed());

        if (!verify) {
            std::memcpy(out_makernote, makerNote, 0xc78);
            return ResultSuccess();
        }

        R_UNLESS(dateStringLength == 0x14, capsrv::ResultInternalFileDataVerificationInvalidDateTimeLength());
        char str[0x20];
        std::snprintf(str, 0x20, "%04d:%02d:%02d %02d:%02d:%02d",
                      file_id.date_time.year,
                      file_id.date_time.month,
                      file_id.date_time.day,
                      file_id.date_time.hour,
                      file_id.date_time.minute,
                      file_id.date_time.second);

        R_UNLESS(std::strncmp(str, dateString, 0x14) == 0, capsrv::ResultInternalFileDataVerificationInconsistentDateTime());
        
        R_UNLESS(true, capsrv::ResultInternalFileDataVerificationInvalidDataDimension()); /* TODO: Width. */
        R_UNLESS(true, capsrv::ResultInternalFileDataVerificationInvalidDataDimension()); /* TODO: Heigh. */

        R_UNLESS(orientation < ams::image::ExifOrientation::Undefined, capsrv::ResultInternalFileDataVerificationInconsistentOrientation());
        R_UNLESS(true, capsrv::ResultInternalFileDataVerificationInconsistentOrientation()); /* TODO: Orientation. */
        R_UNLESS(true, capsrv::ResultInternalFileDataVerificationError()); /* TODO: ? */

        u64 out[2];
        crypto::path::Decrypt(out, (const u64 *)(makerNote + 0x14));
        if (out[1] == 1)
            R_UNLESS(IsExtra(file_id.type), capsrv::ResultAlbumInvalidApplicationId());
        R_UNLESS(out[0] == file_id.application_id, capsrv::ResultInternalFileDataVerificationInconsistentApplicationId());

        std::memcpy(out_makernote, makerNote, 0xc78);

        return ResultSuccess();
    }

    Result ExtractMakerNote(size_t *out_makernote_offset, size_t *out_makernote_size, const u8 *jpeg, size_t jpeg_size) {
        R_UNLESS(jpeg && jpeg_size, capsrv::ResultInternalSignatureMakerNoteExtractionFailed());

        /* TODO: Verify SOI. */
        const size_t tiffSize = jpeg_size - 0xc;
        const u8 *tiff = jpeg + 0xc;
        R_UNLESS(tiff, capsrv::ResultInternalSignatureExifExtractionFailed());

        ams::image::detail::ExifBinary exifBinary;
        ams::image::ExifExtractor exifExtractor(&exifBinary);
        exifExtractor.SetExifData(tiff, tiffSize);
        R_UNLESS(exifExtractor.Analyse(), capsrv::ResultInternalSignatureMakerNoteExtractionFailed());

        u32 makernote_size;
        const u8 *makerNote = exifExtractor.ExtractMakerNote(&makernote_size);
        R_UNLESS(makerNote, capsrv::ResultInternalSignatureMakerNoteExtractionFailed());
        size_t makernote_offset = makerNote - jpeg;

        R_UNLESS(makernote_offset + makernote_size <= jpeg_size, capsrv::ResultAlbumInvalidFileData());

        *out_makernote_offset = makernote_offset;
        *out_makernote_size = makernote_size;

        return ResultSuccess();
    }

    Result VerifyMAC(const u8 *jpeg, size_t jpeg_size, size_t makernote_offset, size_t makernote_size) {
        R_UNLESS(makernote_offset,                                  capsrv::ResultInternalError());
        R_UNLESS(makernote_size,                                    capsrv::ResultInternalError());
        R_UNLESS(jpeg_size,                                         capsrv::ResultInternalError());

        R_UNLESS(makernote_offset <= jpeg_size,                     capsrv::ResultAlbumInvalidFileData());
        R_UNLESS(makernote_size <= jpeg_size - makernote_offset,    capsrv::ResultAlbumInvalidFileData());

        /* TODO: Branch to backwards compatibility. */
        if (true) {
            R_UNLESS(makernote_offset <= 0x11000 - makernote_size,  capsrv::ResultAlbumInvalidFileData());
            R_UNLESS(makernote_size <= 0x1000,                      capsrv::ResultAlbumInvalidFileData());
            R_UNLESS(0x10 <= makernote_size,                        capsrv::ResultAlbumInvalidFileData());
            R_UNLESS(makernote_offset + 0x18 <= jpeg_size,          capsrv::ResultAlbumInvalidFileData());

            u64 mac[2];
            R_TRY(capsrv::crypto::GenerateScreenshotMac(mac, jpeg, jpeg_size, makernote_offset + 8));
            if (std::memcmp(jpeg + makernote_offset + 8, mac, 0x10) != 0)
                return capsrv::ResultInternalFileDataVerificationInconsistentSignature();
        } else {
            R_UNLESS(makernote_offset <= 0x11000 - makernote_size,  capsrv::ResultAlbumInvalidFileData());
            R_UNLESS(makernote_size <= 0x1000,                      capsrv::ResultAlbumInvalidFileData());
            R_UNLESS(0x10 <= makernote_size,                        capsrv::ResultAlbumInvalidFileData());
            R_UNLESS(makernote_offset + 0x18 <= jpeg_size,          capsrv::ResultAlbumInvalidFileData());

            u64 hmac[2];
            capsrv::crypto::ComputeScreenShotHMAC(hmac, jpeg, jpeg_size, makernote_offset + 8);
            if (std::memcmp(jpeg + makernote_offset + 8, hmac, 0x10) != 0)
                return capsrv::ResultInternalFileDataVerificationInconsistentSignature();
        }
        return ResultSuccess();
    }

}