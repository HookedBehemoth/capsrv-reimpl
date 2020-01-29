#include "capsrv_fs.hpp"

#include <vapours/results.hpp>
#include <map>
#include <string>

namespace ams::capsrv {

    constexpr std::string_view mountNames[] =
    {
        [StorageId::Nand] = "NA",
        [StorageId::Sd] =   "SD"
    };

    Result MountImageDirectory(const StorageId storage) {
        FsFileSystem imgDirFs;
        R_TRY(fsOpenImageDirectoryFileSystem(&imgDirFs, FsImageDirectoryId(storage)));

        if (fsdevMountDevice(mountNames[(u8)storage].data(), imgDirFs) == -1) {
            std::abort();
        }

        return ResultSuccess();
    }

    Result MountCustomImageDirectory(const char* path) {
        if (nullptr)
            return fs::ResultNullptrArgument();

        // TODO: Mount Host PC when TMA is a thing.
        return fs::ResultHostFileSystemCorrupted();
    }

    Result UnmountImageDirectory(const StorageId storage) {
        // TODO: Unmount Host PC when TMA is a thing.
        return fsdevUnmountDevice(mountNames[(u8)storage].data());
    }

    std::string_view GetMountName(const StorageId storage) {
        return mountNames[(u8)storage];
    }

}