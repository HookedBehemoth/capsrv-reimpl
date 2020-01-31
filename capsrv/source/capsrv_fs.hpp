#pragma once
#include <stratosphere.hpp>
#include "capsrv_types.hpp"

namespace ams::capsrv {

    Result MountImageDirectory(const StorageId storage);
    Result MountCustomImageDirectory(const char* path);
    Result UnmountImageDirectory(const StorageId storage);
    std::string_view GetMountName(const StorageId storage);

}