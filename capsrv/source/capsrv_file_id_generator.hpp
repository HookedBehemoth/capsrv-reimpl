#pragma once
#include <stratosphere.hpp>

#include "capsrv_album_manager.hpp"
#include "capsrv_types.hpp"

namespace ams::capsrv {

    class AlbumFileIdGenerator {
      private:
        u64 last_timestamp;
        u32 counter;

      public:
        void Initialize();
        void Exit();

        Result GetAutoSavingStorage(StorageId *out, ContentType type, AlbumManager *manager);
        Result GenerateFileId(AlbumFileId *out, u64 application_id, ContentType type, AlbumManager *manager);
    };

}
