#pragma once
#include <stratosphere.hpp>

#include "capsrv_types.hpp"

namespace ams::capsrv::crypto {

    Result Initialize();

    Result GenerateScreenshotMac(u64 *dst, const u8 *in, size_t size, size_t makernote_offset);
    Result GenerateMovieMac(u64 *dst, const u8 *in, size_t size);

    void ComputeScreenShotHMAC(u64 *dst, const u8 *in, size_t size, size_t makernote_offset);

    namespace path {

        struct Cypher {
            u8 c[0x10];
        };

        Cypher EncryptExtraPath(u64 application_id);
        Cypher EncryptFileExtension(u64 application_id, bool is_extra);
        void Decrypt(u64 *dst, const u64 *src);

    }

    namespace application {

        void EncryptV0(ApplicationAlbumEntry *dst, const AlbumEntry *src, const u8 v0Key[0x20]);
        void DecryptV0(AlbumEntry *dst, const ApplicationAlbumEntry *src, const u8 v0Key[0x20]);

        Result EncryptV1(ApplicationAlbumEntry *dst, const AlbumEntry *src, u64 version);
        Result DecryptV1(AlbumEntry *dst, const ApplicationAlbumEntry *src, u64 application_id);

    }

    namespace exif {

        void Decrypt(u8 *ptr, size_t size, size_t offset);

    }

}