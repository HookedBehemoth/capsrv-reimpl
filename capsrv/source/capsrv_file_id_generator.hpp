#pragma once
#include "capsrv_types.hpp"

#include <vapours/results.hpp>

namespace ams::capsrv {

    class FileIdGenerator {
        private:
            u64 lastTimestamp;
            u32 id;
        public:
            FileIdGenerator();

            Result GenerateFileId(FileId *fileId, const u64 appId, const ContentType type);       
    };

}