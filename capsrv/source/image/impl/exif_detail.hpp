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

#pragma once
#include <vapours.hpp>

namespace ams::image::detail {

    typedef void (*ReadU32)(u32 *dst, const u8 *src);
    typedef void (*ReadU16)(u16 *dst, const u8 *src);
    typedef u32 (*GetU32)(const u8 *src);
    typedef u16 (*GetU16)(const u8 *src);

    struct BinaryIo {
        ReadU32 readU32;
        ReadU16 readU16;
        GetU32 getU32;
        GetU16 getU16;
    };

    enum DataFormat : u16 {
        unsignedByte = 1,
        asciiStrings = 2,
        unsignedShort = 3,
        unsignedLong = 4,
        unsignedRational = 5,
        signedByte = 6,
        undefined = 7,
        signedShort = 8,
        signedLong = 9,
        signedRational = 10,
        signedFloat = 11,
        doubleFloat = 12
    };

    struct IfdHeader {
        u16 offset;
        u16 count;
        u16 size;
    };

    struct IfdTag {
        u16 marker;
        DataFormat format;
        union {
            u64 count;
            struct {
                u32 offset;
                u32 size;
            } data;
        };
    };

    struct ExifBinary {
        const u8 *data;
        u32 size;
        BinaryIo binaryIo;
        IfdHeader mainHeader;
        IfdTag mainTags[7] = {
            {0x010f, asciiStrings, 0},    // Make
            {0x0110, asciiStrings, 0},    // Model
            {0x0112, unsignedShort, 1},   // Orientation
            {0x0131, asciiStrings, 0},    // Software
            {0x0132, asciiStrings, 0x14}, // DateTime
            {0x8769, unsignedLong, 1},    // Exif Tag
            {0x8825, unsignedLong, 1},    // GPS Tag
        };
        IfdHeader exifHeader;
        IfdTag exifTags[4] = {
            {0x927c, undefined, 0},       // MakerNote
            {0xa002, unsignedShort, 1},   // Image Width  < N wrongly searches for signedShort
            {0xa003, unsignedShort, 1},   // Image Height < N wrongly searches for signedShort
            {0xa420, asciiStrings, 0x21}, // Image Unique ID
        };
        IfdHeader lastHeader;
        IfdTag lastTags[4] = {
            {0x0103, unsignedShort, 1}, // Compression
            {0x0201, unsignedLong, 1},  // Jpeg IF Offset
            {0x0202, unsignedLong, 1},  // Jpeg IF Byte Count
        };
    };

    bool ReadTiffHeader(BinaryIo *binaryIo, u32 *outOffset, const u8 *tiffHeader, u32 maxSize);
    bool ReadIfdHeader(IfdHeader *ifdHeader, u32 offset, const ExifBinary *exifBinary);
    bool SearchIfdTags(IfdTag *tags, u16 count, const IfdHeader *header, const ExifBinary *exifBinary);

}