#pragma once
#include <stratosphere.hpp>
#include <string>

#include "capsrv_types.hpp"

namespace ams::capsrv::config {

    void Initialize();
    void Exit();

    bool SupportsType(ContentType type);

    u64 GetMax(StorageId storage, ContentType type);
    s64 GetMaxFileSize(ContentType type);
    const char *GetCustomDirectoryPath();

    bool IsDebug();
    bool SetVerification(bool value);
    bool ShouldVerify();

    ALWAYS_INLINE bool ContentTypeValid(ContentType type) {
        return type == ContentType::Screenshot || type == ContentType::Movie || type == ContentType::ExtraScreenshot || type == ContentType::ExtraMovie;
    }

    ALWAYS_INLINE bool StorageValid(StorageId storage) {
        return storage == StorageId::Nand || storage == StorageId::Sd;
    }

}