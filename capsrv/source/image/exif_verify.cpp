#include "exif_verify.hpp"

#include "../capsrv_config.hpp"
#include "../capsrv_crypto.hpp"

namespace ams::capsrv::image {

    Result VerifyMakerNote(u8 *makerNoteOut, const u8 *jpegBuffer, size_t bufferSize, const FileId &fileId, bool verifyMac) {
        bool verify = config::ShouldVerify();
        R_UNLESS(config::SupportsType(fileId.type), capsrv::ResultInvalidContentType());
        R_UNLESS(bufferSize != 0, capsrv::ResultInvalidJPEG());

        if (!verifyMac && verify) {
            size_t makerNoteOffset;
            size_t makerNoteSize;
            R_TRY(ExtractMakerNote(&makerNoteOffset, &makerNoteSize, jpegBuffer, bufferSize));
            R_TRY(VerifyMAC(jpegBuffer, bufferSize, makerNoteOffset, makerNoteSize));
        }

        std::memset(makerNoteOut, 0, 0xc90);

        const u8 *exif_region = nullptr;
        u32 exif_size = 0;
        s32 res = ams::image::detail::ExtractExifRegion(&exif_region, &exif_size, jpegBuffer, bufferSize);
        R_UNLESS(res == 0, capsrv::ResultInvalidJFIF());
        R_UNLESS(exif_region != nullptr, capsrv::ResultInvalidJFIF());
        R_UNLESS(exif_size != 0, capsrv::ResultInvalidJFIF());

        ams::image::detail::ExifBinary exifBinary;
        ams::image::ExifExtractor exifExtractor(&exifBinary);
        exifExtractor.SetExifData(exif_region, exif_size);
        R_UNLESS(exifExtractor.Analyse(), capsrv::ResultInvalidEXIF());
        
        u32 dateStringLength;
        const char *dateString = exifExtractor.ExtractDateTime(&dateStringLength);
        R_UNLESS((dateString && dateStringLength) || !verify, capsrv::ResultMissingDateTime());

        ams::image::ExifOrientation orientation;
        bool success = exifExtractor.ExtractOrientation(&orientation);
        if (!success)
            orientation = ams::image::ExifOrientation::UpperLeft;

        u32 makerNoteSize;
        const u8 *makerNote = exifExtractor.ExtractMakerNote(&makerNoteSize);
        R_UNLESS((makerNote && makerNoteSize) || !verify, capsrv::ResultMissingMakerNote());

        if (!verify) {
            std::memcpy(makerNoteOut, makerNote, 0xc78);
            return ResultSuccess();
        }

        R_UNLESS(dateStringLength == 0x14, capsrv::ResultInvalidDateTimeLength());
        char str[0x14];
        std::snprintf(str, 0x14, "%04d:%02d:%02d %02d:%02d:%02d",
                      fileId.datetime.year,
                      fileId.datetime.month,
                      fileId.datetime.day,
                      fileId.datetime.hour,
                      fileId.datetime.minute,
                      fileId.datetime.second);

        R_UNLESS(std::strncmp(str, dateString, 0x14) == 0, capsrv::ResultDateTimeMissmatch());
        
        R_UNLESS(true, capsrv::ResultInvalidDimension()); /* TODO: Dimensions. */

        R_UNLESS(orientation < ams::image::ExifOrientation::Undefined, capsrv::ResultInvalidOrientation());
        R_UNLESS(true, capsrv::ResultInvalidOrientation()); /* TODO: Orientation. */
        R_UNLESS(true, capsrv::ResultMakerNoteUnk()); /* TODO: ? */

        u64 out[2];
        crypto::aes128::Decrypt(out, (const u64 *)(makerNote + 0x14));
        if (out[1] == 1)
            R_UNLESS(fileId.IsExtra(), capsrv::ResultInvalidUnknown());
        R_UNLESS(out[0] == fileId.applicationId, capsrv::ResultApplicationIdMissmatch());

        std::memcpy(makerNoteOut, makerNote, 0xc78);

        return ResultSuccess();
    }

    Result ExtractMakerNote(size_t *makerNoteOffsetOut, size_t *makerNoteSizeOut, const u8 *jpegBuffer, size_t bufferSize) {
        R_UNLESS(jpegBuffer && bufferSize, capsrv::ResultInvalidJpegSize());
        /* TODO: Verify header magic. */
        const size_t tiffSize = bufferSize - 0xc;
        const u8 *tiff = jpegBuffer + 0xc;
        R_UNLESS(tiff, capsrv::ResultInvalidJpegHeader());

        ams::image::detail::ExifBinary exifBinary;
        ams::image::ExifExtractor exifExtractor(&exifBinary);
        exifExtractor.SetExifData(tiff, tiffSize);
        R_UNLESS(exifExtractor.Analyse(), capsrv::ResultInvalidJpegSize());

        u32 makerNoteSize;
        const u8 *makerNote = exifExtractor.ExtractMakerNote(&makerNoteSize);
        R_UNLESS(makerNote, capsrv::ResultInvalidJpegSize());
        size_t makerNoteOffset = makerNote - jpegBuffer;

        R_UNLESS(makerNoteOffset + makerNoteSize <= bufferSize, capsrv::ResultInvalidFileData());

        *makerNoteOffsetOut = makerNoteOffset;
        *makerNoteSizeOut = makerNoteSize;

        return ResultSuccess();
    }

    Result VerifyMAC(const u8 *jpegBuffer, size_t jpegSize, size_t makerNoteOffset, size_t makerNoteSize) {
        R_UNLESS(makerNoteOffset,                               capsrv::ResultInvalidMakerNote());
        R_UNLESS(makerNoteSize,                                 capsrv::ResultInvalidMakerNote());
        R_UNLESS(jpegSize,                                      capsrv::ResultInvalidMakerNote());

        R_UNLESS(makerNoteOffset <= jpegSize,                   capsrv::ResultInvalidFileData());
        R_UNLESS(makerNoteSize <= jpegSize - makerNoteOffset,   capsrv::ResultInvalidFileData());

        /* TODO: Branch to backwards compatibility. */
        if (true) {
            R_UNLESS(makerNoteOffset <= 0x11000 - makerNoteSize,    capsrv::ResultInvalidFileData());
            R_UNLESS(makerNoteSize <= 0x1000,                       capsrv::ResultInvalidFileData());
            R_UNLESS(0x10 <= makerNoteSize,                         capsrv::ResultInvalidFileData());
            R_UNLESS(makerNoteOffset + 0x18 <= jpegSize,            capsrv::ResultInvalidFileData());

            u64 mac[2];
            R_TRY(capsrv::crypto::GenerateScreenshotMac(mac, jpegBuffer, jpegSize, makerNoteOffset + 8));
            if (std::memcmp(jpegBuffer + makerNoteOffset + 8, mac, 0x10) != 0)
                return capsrv::ResultInvalidMacHash();
        } else {
            R_UNLESS(makerNoteOffset <= 0x11000 - makerNoteSize,    capsrv::ResultInvalidFileData());
            R_UNLESS(makerNoteSize <= 0x1000,                       capsrv::ResultInvalidFileData());
            R_UNLESS(0x10 <= makerNoteSize,                         capsrv::ResultInvalidFileData());
            R_UNLESS(makerNoteOffset + 0x18 <= jpegSize,            capsrv::ResultInvalidFileData());

            u64 hmac[2];
            capsrv::crypto::ComputeScreenShotHMAC(hmac, jpegBuffer, jpegSize, makerNoteOffset + 8);
            if (std::memcmp(jpegBuffer + makerNoteOffset + 8, hmac, 0x10) != 0)
                return capsrv::ResultInvalidMacHash();
        }
        return ResultSuccess();
    }

}