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
#include "exif_types.hpp"
#include "impl/exif_detail.hpp"

namespace ams::image {

    class ExifExtractor {
    public:
        ExifExtractor(detail::ExifBinary *exifBinary);

        void SetExifData(const u8 *data, u64 size);
        bool Analyse();

        const char *ExtractDateTime(u32 *size);
        const char* ExtractMaker(u32 *size);
        const u8* ExtractMakerNote(u32 *size);
        bool ExtractOrientation(ExifOrientation *orientation);
        const u8* ExtractThumbnail(u32 *size);
    private:
        int state;
        const u8 *data;
        u64 size;
        detail::ExifBinary *exifBinary;
    };

};