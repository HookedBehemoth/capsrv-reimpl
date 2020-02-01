#pragma once
#include <stratosphere.hpp>
#include "capsrv_types.hpp"

namespace ams::capsrv {

    Result MountImageDirectory(StorageId storage);
    Result MountCustomImageDirectory(const char* path);
    Result UnmountImageDirectory(StorageId storage);
    std::string_view GetMountName(StorageId storage);

}