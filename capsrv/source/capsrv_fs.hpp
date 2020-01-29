#pragma once
#include "capsrv_types.hpp"

#include <vapours/results.hpp>
#include <switch.h>

namespace ams::capsrv {

    Result MountImageDirectory(const StorageId storage);
    Result MountCustomImageDirectory(const char* path);
    Result UnmountImageDirectory(const StorageId storage);
    std::string GetMountName(const StorageId storage);

}