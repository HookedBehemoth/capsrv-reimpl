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

#include "image_types.hpp"

namespace ams::image {

    class ExifBuilder {
      private:
        ProcessStage stage;
        void *work_buffer;

      public:
        size_t GetWorkBufferSize();

        ExifBuilder(void *work_buffer, size_t work_buffer_size);
        virtual ~ExifBuilder();

        void SetOrientation(ExifOrientation orientation);
        void SetSoftware(const char *software, size_t length);
        void SetDateTime(const char *date_time, size_t length);
        void SetMakerNote(const void *maker_note, size_t length);
        void SetUniqueId(const char *unique_id, size_t length);
        void SetThumbnail(const void *thumbnail, size_t size);
    };

}