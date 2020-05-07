#pragma once

#include <stratosphere.hpp>

namespace ams::capsrv {

    enum class StorageId : u8 {
        Nand = 0,
        Sd =   1,

        Count,
    };

    enum class MountState : u8 {
        unmounted,
        available,
        mounted,
    };

    constexpr const char *const MountPoints[2]{
        "NA:/",
        "SD:/",
    };

    constexpr const char *const MountNames[2]{
        "NA",
        "SD",
    };

    ALWAYS_INLINE bool StorageValid(StorageId storage) {
        return storage == StorageId::Nand || storage == StorageId::Sd;
    }

    static Result VerifyStorageId(StorageId storage) {
        if (StorageValid(storage)) {
            return ResultSuccess();
        } else {
            return capsrv::ResultAlbumInvalidStorage();
        }
    }

}
