/*
 * Copyright (c) 2018-2020 Atmosphère-NX
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
#include "fs_image_directory.hpp"
#include "../capsrv_results.hpp"

namespace ams::fs {

    namespace impl {
        /* Content storage names. */
        constexpr inline const char * const ImageDirectoryNandMountName     = "NA";
        constexpr inline const char * const ImageDirectorySdCardMountName   = "SD";
    }

    namespace {

        class ImageDirectoryCommonMountNameGenerator : public fsa::ICommonMountNameGenerator, public impl::Newable {
            private:
                const ImageDirectoryId id;
            public:
                explicit ImageDirectoryCommonMountNameGenerator(ImageDirectoryId i) : id(i) { /* ... */ }

                virtual Result GenerateCommonMountName(char *dst, size_t dst_size) override {
                    /* Determine how much space we need. */
                    const size_t needed_size = strnlen(GetImageDirectoryMountName(id), MountNameLengthMax) + 2;
                    AMS_ABORT_UNLESS(dst_size >= needed_size);

                    /* Generate the name. */
                    auto size = std::snprintf(dst, dst_size, "%s:", GetImageDirectoryMountName(id));
                    AMS_ASSERT(static_cast<size_t>(size) == needed_size - 1);

                    return ResultSuccess();
                }
        };

    }

    const char *GetImageDirectoryMountName(ImageDirectoryId id) {
        switch (id) {
            case ImageDirectoryId::Nand:   return impl::ImageDirectoryNandMountName;
            case ImageDirectoryId::SdCard: return impl::ImageDirectorySdCardMountName;
            AMS_UNREACHABLE_DEFAULT_CASE();
        }
    }

    Result MountImageDirectory(ImageDirectoryId id) {
        return MountImageDirectory(GetImageDirectoryMountName(id), id);
    }

    Result MountImageDirectory(const char *name, ImageDirectoryId id) {
        /* Validate the mount name. */
        //R_TRY(impl::CheckMountNameAllowingReserved(name));

        /* Mount the content storage, use libnx bindings. */
        ::FsFileSystem fs;
        R_TRY(fsOpenImageDirectoryFileSystem(std::addressof(fs), static_cast<::FsImageDirectoryId>(id)));

        /* Allocate a new filesystem wrapper. */
        auto fsa = std::make_unique<RemoteFileSystem>(fs);
        R_UNLESS(fsa != nullptr, capsrv::ResultAllocationFailureInImageDirectoryA());

        /* Allocate a new mountname generator. */
        auto generator = std::make_unique<ImageDirectoryCommonMountNameGenerator>(id);
        R_UNLESS(generator != nullptr, capsrv::ResultAllocationFailureInImageDirectoryB());

        /* Register. */
        return fsa::Register(name, std::move(fsa), std::move(generator));
    }

}
