#include "capsrv_crypto.hpp"

namespace ams::capsrv::crypto {

    namespace {

        /* Key source for screenshot key. */
        constexpr u8 screenshot_kek_source[0x10] = {0x9b, 0xbf, 0xde, 0x01, 0xfa, 0x18, 0xbb, 0xee, 0x87, 0xa4, 0xac, 0xc5, 0x67, 0xac, 0xd4, 0x21};
        constexpr u8 screenshot_key_source[0x10] = {0x15, 0x0d, 0x26, 0xd1, 0x17, 0xee, 0x44, 0xff, 0x79, 0x67, 0x43, 0x8a, 0x20, 0x14, 0x63, 0x30};
        u8 screenshot_kek[0x10] = {};

        /* Key source for movie key. */
        constexpr u8 movie_kek_source[0x10] = {0x55, 0xb4, 0x30, 0xaf, 0x16, 0x8d, 0x9a, 0x66, 0x6a, 0x8f, 0xed, 0xe3, 0xa5, 0x52, 0x32, 0xea};
        constexpr u8 movie_key_source[0x10] = {0x71, 0x70, 0x7d, 0xed, 0x3e, 0x2f, 0x48, 0x03, 0x37, 0x2f, 0x42, 0x35, 0x12, 0x39, 0x2c, 0xfe};
        u8 movie_kek[0x10] = {};

        /* Unofficial key source for decrypting official keys. */
        constexpr u8 aes_kek_source[0x10] = {0xE2, 0x41, 0x1A, 0xA8, 0x2E, 0x55, 0xEB, 0x16, 0xF2, 0xF7, 0x82, 0xFB, 0x44, 0x10, 0xFE, 0xE5};
        constexpr u8 aes_key_source[0x10] = {0x8C, 0xFB, 0x07, 0x94, 0xC8, 0x1C, 0xD3, 0x88, 0x80, 0x71, 0x01, 0x98, 0xAC, 0xB9, 0x13, 0xB9};

        /* HMAC-SHA256 key source. */
        constexpr u8 hmac_encrypted_key[0x20] = {0x56, 0x13, 0xA8, 0xAF, 0x2C, 0x46, 0xF2, 0x0D, 0xC6, 0x74, 0x4A, 0xA2, 0x3A, 0x51, 0x09, 0x52, 0xEC, 0xBE, 0x03, 0xF1, 0x46, 0x2A, 0x95, 0x79, 0x79, 0x0A, 0xD7, 0x01, 0x93, 0x34, 0x80, 0xDA};
        u8 hmac_key[0x20] = {};

        /* AES 128 key for path names. */
        constexpr u8 path_encrypted_key[0x10] = {0xD9, 0x63, 0x84, 0x08, 0x48, 0x9B, 0x43, 0xDB, 0x79, 0x50, 0xDC, 0x3D, 0x32, 0x92, 0x2B, 0x3A};
        u8 path_key[0x10] = {};

        /* AES 128 key for exif decryption. */
        constexpr u8 exif_encrypted_key[0x10] = {0xC3, 0x3F, 0x45, 0xBE, 0xF7, 0x10, 0x19, 0x50, 0x42, 0x26, 0x95, 0xD7, 0xDC, 0x48, 0x6E, 0x5E};
        u8 exif_key[0x10] = {};

        /* AES 256 key for application entries. */
        constexpr u8 application_encrypted_key[0x20] = {0x38, 0x5E, 0x44, 0xF9, 0x60, 0xFB, 0x37, 0x10, 0xEA, 0x2A, 0xD3, 0xC8, 0x40, 0xB8, 0x89, 0xA2, 0x34, 0x7B, 0xFD, 0xA3, 0xBC, 0xB5, 0x03, 0xD0, 0xCE, 0xD0, 0xE5, 0x81, 0x6F, 0x4D, 0x8B, 0xB8};
        u8 application_key[0x20] = {};

        Result GenerateMac(u64 *dst, const u8 *in, size_t size, const u8 kek[0x10], const u8 key_source[0x10]) {
            u32 keyslot;
            R_TRY(splCryptoLockAesEngine(&keyslot));
            ON_SCOPE_EXIT {
                splCryptoUnlockAesEngine(keyslot);
            };
            R_TRY(splCryptoLoadAesKey(kek, key_source, keyslot));
            R_TRY(splCryptoComputeCmac(in, size, keyslot, dst));
            return ResultSuccess();
        }

    }

    Result Initialize() {
        /* Generate MAC keys for signing and validating album files. */
        R_TRY(splCryptoGenerateAesKek(screenshot_kek_source, 0, 0, screenshot_kek));
        R_TRY(splCryptoGenerateAesKek(movie_kek_source, 0, 0, movie_kek));

        /* Generate random aes key that was used to encrypt official AES keys. */
        u8 aes_kek[0x10] = {};
        u8 aes_aes_key[0x10] = {};
        R_TRY(splCryptoGenerateAesKek(aes_kek_source, 0, 0, aes_kek));
        R_TRY(splCryptoGenerateAesKey(aes_kek, aes_key_source, aes_aes_key));

        /* Decrypt official keys. */
        Aes128Context ctx;
        aes128ContextCreate(&ctx, aes_aes_key, 0);
        aes128DecryptBlock(&ctx, exif_key, exif_encrypted_key);
        aes128DecryptBlock(&ctx, path_key, path_encrypted_key);
        aes128DecryptBlock(&ctx, application_key, application_encrypted_key);
        aes128DecryptBlock(&ctx, application_key + 0x10, application_encrypted_key + 0x10);
        aes128DecryptBlock(&ctx, hmac_key, hmac_encrypted_key);
        aes128DecryptBlock(&ctx, hmac_key + 0x10, hmac_encrypted_key + 0x10);

        return ResultSuccess();
    }

    Result GenerateScreenshotMac(u64 *dst, const u8 *in, size_t size, size_t makerNoteOffset) {
        u8 nil[0x10]{};
        ams::crypto::Sha256Generator sha256;
        sha256.Initialize();
        sha256.Update(nil, 0x10);
        sha256.Update(in + makerNoteOffset + 0x10, size - makerNoteOffset + 0x10);
        u8 hash[0x20];
        sha256.GetHash(&hash, 0x20);

        u64 mac[2];
        R_TRY(GenerateMac(mac, hash, 0x20, screenshot_kek, screenshot_key_source));

        dst[0] = mac[0];
        dst[1] = mac[1];

        return ResultSuccess();
    }

    Result GenerateMovieMac(u64 *dst, const u8 *in, size_t size) {
        return GenerateMac(dst, in, size, movie_kek, movie_key_source);
    }

    void ComputeScreenShotHMAC(u64 *dst, const u8 *in, size_t size, size_t makerNoteOffset) {
        size_t off = makerNoteOffset + 0x10;
        u8 nil[0x10]{};
        HmacSha256Context ctx;
        hmacSha256ContextCreate(&ctx, hmac_key, 0x20);
        hmacSha256ContextUpdate(&ctx, in, makerNoteOffset);
        hmacSha256ContextUpdate(&ctx, nil, 0x10);
        hmacSha256ContextUpdate(&ctx, in + off, size - off);

        u64 mac[4];
        hmacSha256ContextGetMac(&ctx, mac);

        dst[0] = mac[0];
        dst[1] = mac[1];
    }

    namespace path {

        Cypher EncryptExtraPath(u64 application_id) {
            u64 in[2] = {application_id, 0};
            Cypher out;
            Aes128Context ctx;
            aes128ContextCreate(&ctx, path_key, true);
            aes128EncryptBlock(&ctx, out.c, in);
            return out;
        }

        Cypher EncryptFileExtension(u64 application_id, bool is_extra) {
            u64 in[2] = {application_id, 0};
            ((u8*)in)[0xf] = is_extra;
            Cypher out;

            Aes128Context ctx;
            aes128ContextCreate(&ctx, path_key, true);
            aes128EncryptBlock(&ctx, out.c, in);
            return out;
        }

        void Decrypt(u64 *dst, const u64 *src) {
            Aes128Context ctx;
            aes128ContextCreate(&ctx, path_key, false);
            aes128DecryptBlock(&ctx, dst, src);
        }

    }

    namespace application {

        void EncryptV0(ApplicationAlbumEntry *dst, const AlbumEntry *src, const u8 v0Key[0x20]) {
            Aes256Context ctx;
            aes256ContextCreate(&ctx, v0Key, 1);
            aes256EncryptBlock(&ctx, dst->data, ((u8 *)src));
            aes256EncryptBlock(&ctx, dst->data + 0x10, ((u8 *)src) + 0x10);
        }

        void DecryptV0(AlbumEntry *dst, const ApplicationAlbumEntry *src, const u8 v0Key[0x20]) {
            Aes256Context ctx;
            aes256ContextCreate(&ctx, v0Key, 0);
            aes256DecryptBlock(&ctx, (u8 *)dst, (u8 *)src);
            aes256DecryptBlock(&ctx, (u8 *)dst + 0x10, (u8 *)src + 0x10);
        }

        Result EncryptV1(ApplicationAlbumEntry *dst, const AlbumEntry *src, u64 version) {
            R_UNLESS(version == 1, 0x800ce);
            u64 tmp[4];
            AlbumEntry tmpEntry = *src;
            tmpEntry.fileId.pad[0x5] = 1;
            Aes256Context ctx;
            aes256ContextCreate(&ctx, application_key, 1);
            aes256EncryptBlock(&ctx, tmp, ((u8 *)&tmpEntry));
            aes256EncryptBlock(&ctx, tmp + 2, ((u8 *)&tmpEntry) + 0x10);
            dst->v1.size = src->size;
            dst->v1.hash = tmp[0] ^ tmp[1] ^ tmp[2] ^ tmp[3];
            dst->v1.date_time = src->fileId.date_time;
            dst->v1.storage_id = src->fileId.storage_id;
            dst->v1.type = src->fileId.type;
            dst->v1.unk_x1f = 1;
            return ResultSuccess();
        }

        Result DecryptV1(AlbumEntry *dst, const ApplicationAlbumEntry *src, u64 application_id) {
            R_UNLESS(src->v1.unk_x1f == '\x01', capsrv::ResultAlbumInvalidApplicationId());

            /* Mimic valid Entry. */
            u64 tmp[4];
            ApplicationAlbumEntry tmpEntry = *src;
            tmpEntry.v1.hash = application_id;
            Aes256Context ctx;
            aes256ContextCreate(&ctx, application_key, 1);
            aes256EncryptBlock(&ctx, tmp, ((u8 *)&tmpEntry));
            aes256EncryptBlock(&ctx, tmp + 2, ((u8 *)&tmpEntry) + 0x10);

            /* Verify that hashs match. */
            R_UNLESS(src->v1.hash == (tmp[0] ^ tmp[1] ^ tmp[2] ^ tmp[3]), capsrv::ResultAlbumInvalidApplicationId());

            dst->size = src->v1.size;
            dst->fileId.application_id = application_id;
            dst->fileId.date_time = src->v1.date_time;
            dst->fileId.storage_id = src->v1.storage_id;
            dst->fileId.type = src->v1.type;
            for (int i = 0; i < 6; i++)
                dst->fileId.pad[i] = 0;

            return ResultSuccess();
        }

    }

    namespace exif {

        void Decrypt(u8 *ptr, size_t size, size_t offset) {
            u8 iv[0x10] = {0x34, 0x62, 0xbd, 0x0e, 0xbe, 0x71, 0x82, 0x74, 0x28, 0xb9, 0x7c, 0x3b, 0x29, 0xbf, 0xdf, 0x97};
            ams::crypto::DecryptAes128CtrPartial(ptr, size - offset, exif_key, sizeof(exif_key), iv, sizeof(iv), offset, ptr, size);
        }

    }

}