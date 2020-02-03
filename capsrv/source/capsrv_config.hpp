#pragma once
#include <stratosphere.hpp>
#include "capsrv_types.hpp"

#include <string>

namespace ams::capsrv::config {

    void Initialize();
    void Exit();

    bool SupportsType(ContentType type);
    bool StorageValid(StorageId storage);
    inline u64 GetMax(StorageId storage, ContentType type);
    const char* GetCustomDirectoryPath();
}