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
#include "exif_detail.hpp"

namespace ams::image::detail {

    namespace {
        constexpr u16 MOTOROLA_MAGIC = 0x4d4d;
        constexpr u16 INTEL_MAGIC = 0x4949;

        constexpr u8 DataFormatSize[12] = {1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8};
    }

    namespace motorola {
        void ReadU32(u32 *dst, const u8 *src) {
            *dst = src[0] << 0x18 | src[1] << 0x10 | src[2] << 0x8 | src[3];
        }

        void ReadU16(u16 *dst, const u8 *src) {
            *dst = src[0] << 0x8 | src[1];
        }

        u32 GetU32(const u8 *src) {
            return src[0] << 0x18 | src[1] << 0x10 | src[2] << 0x8 | src[3];
        }

        u16 GetU16(const u8 *src) {
            return src[0] << 0x8 | src[1];
        }
    }

    namespace intel {
        void ReadU32(u32 *dst, const u8 *src) {
            *dst = *(u32 *)src;
        }

        void ReadU16(u16 *dst, const u8 *src) {
            *dst = *(u16 *)src;
        }

        u32 GetU32(const u8 *src) {
            return *(u32 *)src;
        }

        u16 GetU16(const u8 *src) {
            return *(u16 *)src;
        }
    }

    bool ReadTiffHeader(BinaryIo *binaryIo, u32 *outOffset, const u8 *ptr, u32 maxSize) {
        if (*(u16 *)ptr == MOTOROLA_MAGIC) {
            binaryIo->readU32 = motorola::ReadU32;
            binaryIo->readU16 = motorola::ReadU16;
            binaryIo->getU32 = motorola::GetU32;
            binaryIo->getU16 = motorola::GetU16;
        } else if (*(u16 *)ptr == INTEL_MAGIC) {
            binaryIo->readU32 = intel::ReadU32;
            binaryIo->readU16 = intel::ReadU16;
            binaryIo->getU32 = intel::GetU32;
            binaryIo->getU16 = intel::GetU16;
        } else {
            return false;
        }

        u16 tagMark = binaryIo->getU16(ptr + 2);
        if (tagMark != 0x2A)
            return false;

        u32 offset = binaryIo->getU32(ptr + 4);
        if (offset < 8 || offset > maxSize - 6)
            return false;

        *outOffset = offset;
        return true;
    }

    bool ReadIfdHeader(IfdHeader *ifdHeader, u32 offset, const ExifBinary *exifBinary) {
        if (offset < 8 || offset > exifBinary->size - 6)
            return false;

        u16 count = exifBinary->binaryIo.getU16(exifBinary->data + offset);
        u16 headerSize = (count * 3) * 4;

        if (offset + headerSize + 6 > exifBinary->size)
            return false;

        u16 ifdSize = exifBinary->binaryIo.getU32(exifBinary->data + (headerSize | 2) + offset);

        if (ifdSize != 0 && (ifdSize < 8 || ifdSize > exifBinary->size - 6))
            return false;

        ifdHeader->offset = offset;
        ifdHeader->count = count;
        ifdHeader->size = ifdSize;
        return true;
    }

    bool SearchIfdTags(IfdTag *tags, u16 count, const IfdHeader *header, const ExifBinary *exifBinary) {
        if (count == 0)
            return true;

        for (u16 entryIndex = 0; entryIndex < header->count; entryIndex++) {
            u16 offset = entryIndex * 0xc + 2 + header->offset;
            u16 marker = exifBinary->binaryIo.getU16(exifBinary->data + offset);
            u16 tagIndex;
            for (tagIndex = 0; tagIndex < count; tagIndex++) {
                if (marker == tags[tagIndex].marker)
                    break;
            }
            if (marker != tags[tagIndex].marker) {
                continue;
            }
            u16 format = exifBinary->binaryIo.getU16(exifBinary->data + offset + 2);
            if (format != tags[tagIndex].format) {
                continue;
            }
            u32 componentNumber = exifBinary->binaryIo.getU32(exifBinary->data + offset + 4);
            if (componentNumber >= 0xffff || (tags[tagIndex].count != 0 && tags[tagIndex].count != componentNumber)) {
                continue;
            }
            u8 formatSize = DataFormatSize[format - 1];
            if (formatSize * componentNumber <= 4) {
                tags[tagIndex].data.offset = offset + 0x8;
            } else {
                tags[tagIndex].data.offset = exifBinary->binaryIo.getU32(exifBinary->data + offset + 0x8);
            }
            tags[tagIndex].data.size = formatSize * componentNumber;
        }
        return true;
    }

}
