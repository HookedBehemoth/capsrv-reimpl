#pragma once

#include <stratosphere.hpp>
#include "capsrv_types.hpp"

namespace ams::capsrv {

    class ApplicationAlbumManager {
      private:
        struct ApplicationResource {
            bool used;
            u64 aruid;
            u64 application_id;
            u64 version;
            u8 key[0x20];

            Result MakeAlbumEntry(AlbumEntry *out_entry, const ApplicationAlbumEntry &entry);
        };

        bool initialized;
        os::Mutex mutex;
        ApplicationResource resources[4];

      public:
        ApplicationAlbumManager();

        void Initialize();
        void Exit();

        Result RegisterAppletResourceUserId(u64 aruid, u64 application_id);
        void UnregisterAppletResourceUserId(u64 aruid, u64 application_id);

        Result GetApplicationIdFromAruid(u64 *application_id, u64 aruid);
        Result CheckApplicationIdRegistered(u64 application_id);

        Result SetShimLibraryVersion(u64 version, u64 aruid);

        Result GenerateApplicationAlbumEntry(ApplicationAlbumEntry *appEntry, const AlbumEntry &entry, u64 application_id);

        Result GetAlbumEntryFromApplicationAlbumEntry(AlbumEntry *out, const ApplicationAlbumEntry *src, u64 application_id);
        Result GetAlbumEntryFromApplicationAlbumEntryAruid(AlbumEntry *out, const ApplicationAlbumEntry *src, u64 aruid);

        /* AppAlbumEntryToAlbumEntry? */

        Result EncryptAaeBuffer(ApplicationAlbumEntry *appEntry, u64 count, u64 aruid);
        Result EncryptAafeBuffer(ApplicationAlbumFileEntry *appFileEntry, u64 count, u64 aruid);
    };

}
