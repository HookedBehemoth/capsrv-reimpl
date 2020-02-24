/*
 * Copyright (c) 2018-2019 Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "exif_extractor.hpp"

namespace ams::image {

    ExifExtractor::ExifExtractor(detail::ExifBinary *exifBinary) {
        this->state = 0;
        this->exifBinary = exifBinary;
    }

    void ExifExtractor::SetExifData(const u8 *data, u64 size) {
        this->data = data;
        this->size = size;
        this->state = 1;
    }

    bool ExifExtractor::Analyse() {
        if (this->state < 1)
            return false;
            
        detail::ExifBinary *exifBinary = this->exifBinary;
        exifBinary->data = this->data;
        exifBinary->size = this->size;
        
        u32 offset = 0;
        bool success = detail::ReadTiffHeader(&exifBinary->binaryIo, &offset, this->data, this->size);
        if (!success)
            return false;
        success = detail::ReadIfdHeader(&exifBinary->mainHeader, offset, exifBinary);
        if (!success)
            return false;
        success = detail::SearchIfdTags(exifBinary->mainTags, 7, &exifBinary->mainHeader, exifBinary);
        if (!success)
            return false;
        u32 exifTagDataOffset = exifBinary->mainTags[5].data.offset; // Exif Tag
        if (exifTagDataOffset == 0) {
            exifBinary->exifTags[0].count = 0;
            exifBinary->exifTags[1].count = 0;
            exifBinary->exifTags[2].count = 0;
            exifBinary->exifTags[3].count = 0;
        } else {
            u32 exifOffset = exifBinary->binaryIo.getU32(exifBinary->data + exifTagDataOffset);
            if (exifBinary->size <= exifOffset)
                return false;
            success = detail::ReadIfdHeader(&exifBinary->exifHeader, exifOffset, exifBinary);
            if (!success)
                return false;
            success = detail::SearchIfdTags(exifBinary->exifTags, 4, &exifBinary->exifHeader, exifBinary);
            if (!success)
                return false;
        }
        u32 exifSize = exifBinary->mainHeader.size;
        if (exifSize == 0) {
            exifBinary->lastTags[0].count = 0;
            exifBinary->lastTags[1].count = 0;
            exifBinary->lastTags[2].count = 0;
        } else {
            success = detail::ReadIfdHeader(&exifBinary->lastHeader, exifSize, exifBinary);
            if (!success)
                return false;
            success = detail::SearchIfdTags(exifBinary->lastTags, 3, &exifBinary->lastHeader, exifBinary);
            if (!success)
                return false;
        }
        this->state = 2;
        return 0;
    }
    
    const char *ExifExtractor::ExtractDateTime(u32 *size) {
        if (this->exifBinary->mainTags[4].data.size != 0x14)
            return nullptr;

        const char *dateString = (const char*)this->exifBinary->data + this->exifBinary->mainTags[4].data.offset;

        for (u32 i = 0; i < 0x13; i++)
            if (dateString[i] == '\0')
                return nullptr;

        if (dateString[0x13] != '\0')
            return nullptr;

        *size = 0x14;
        return dateString;
    }

    const char* ExifExtractor::ExtractMaker(u32 *size) {
        auto makerSize = this->exifBinary->mainTags[0].data.size;
        if (makerSize == 0)
            return nullptr;

        const char *makerString = (const char*)this->exifBinary->data + this->exifBinary->mainTags[0].data.offset;

        if (makerSize != 1) {
            for (u32 i = 0; i < makerSize - 1; i++) {
                if (makerString[i] == '\0')
                    return nullptr;
            }
        }
        if (makerString[makerSize - 1] != '\0')
            return nullptr;

        *size = makerSize;
        return makerString;
    }

    const u8* ExifExtractor::ExtractMakerNote(u32 *size) {
        auto mnSize = this->exifBinary->exifTags[0].data.size;
        
        if (mnSize == 0)
            return nullptr;

        *size = mnSize;
        return this->exifBinary->data + this->exifBinary->exifTags[0].data.offset;
    }

    const u8* ExifExtractor::ExtractThumbnail(u32 *size) {
        if (this->exifBinary->lastTags[0].data.size != 2 || this->exifBinary->lastTags[1].data.size != 4 || this->exifBinary->lastTags[2].data.size != 4)
            return nullptr;
        
        u16 compression = this->exifBinary->binaryIo.getU16(this->exifBinary->data + this->exifBinary->lastTags[0].data.offset);

        if (compression != 6) // JPEG
            return nullptr;
        
        u32 jpegOffset = this->exifBinary->binaryIo.getU32(this->exifBinary->data + this->exifBinary->lastTags[1].data.offset);
        u32 jpegSize = this->exifBinary->binaryIo.getU32(this->exifBinary->data + this->exifBinary->lastTags[2].data.offset);

        u32 max = this->exifBinary->size;
        if (jpegOffset + jpegSize > max || jpegOffset > max || jpegSize > max)
            return nullptr;
        
        *size = jpegSize;
        return this->exifBinary->data + jpegOffset;
    }

    bool ExifExtractor::ExtractOrientation(ExifOrientation *orientation) {
        if (this->exifBinary->mainTags[2].data.size != 2)
            return false;

        auto num = this->exifBinary->binaryIo.getU16(this->exifBinary->data + this->exifBinary->mainTags[2].data.offset);
        if (num >= 9)
            return false;

        *orientation = ExifOrientation(num);
        return true;
    }
}