#include "capsrv_crypto.hpp"

namespace ams::capsrv::crypto {

    namespace aes128 {
        constexpr u8 encrypted_key[0x10] = {0xD9, 0x63, 0x84, 0x08, 0x48, 0x9B, 0x43, 0xDB, 0x79, 0x50, 0xDC, 0x3D, 0x32, 0x92, 0x2B, 0x3A};
        static u8 key[0x10] = {0};
    }

    namespace aes256 {
        constexpr u8 encrypted_key[0x20] = {0x38, 0x5E, 0x44, 0xF9, 0x60, 0xFB, 0x37, 0x10, 0xEA, 0x2A, 0xD3, 0xC8, 0x40, 0xB8, 0x89, 0xA2, 0x34, 0x7B, 0xFD, 0xA3, 0xBC, 0xB5, 0x03, 0xD0, 0xCE, 0xD0, 0xE5, 0x81, 0x6F, 0x4D, 0x8B, 0xB8};
        static u8 key[0x20] = {0};
    }

    namespace {

        constexpr u8 screenshot_kek_source[0x10] = {0x9b, 0xbf, 0xde, 0x01, 0xfa, 0x18, 0xbb, 0xee, 0x87, 0xa4, 0xac, 0xc5, 0x67, 0xac, 0xd4, 0x21};
        constexpr u8 screenshot_key_source[0x10] = {0x15, 0x0d, 0x26, 0xd1, 0x17, 0xee, 0x44, 0xff, 0x79, 0x67, 0x43, 0x8a, 0x20, 0x14, 0x63, 0x30};
        static u8 screenshot_kek[0x10] = {0};

        constexpr u8 movie_kek_source[0x10] = {0x55, 0xb4, 0x30, 0xaf, 0x16, 0x8d, 0x9a, 0x66, 0x6a, 0x8f, 0xed, 0xe3, 0xa5, 0x52, 0x32, 0xea};
        constexpr u8 movie_key_source[0x10] = {0x71, 0x70, 0x7d, 0xed, 0x3e, 0x2f, 0x48, 0x03, 0x37, 0x2f, 0x42, 0x35, 0x12, 0x39, 0x2c, 0xfe};
        static u8 movie_kek[0x10] = {0};

        constexpr u8 aes_kek_source[0x10] = {0xE2, 0x41, 0x1A, 0xA8, 0x2E, 0x55, 0xEB, 0x16, 0xF2, 0xF7, 0x82, 0xFB, 0x44, 0x10, 0xFE, 0xE5};
        constexpr u8 aes_key_source[0x10] = {0x8C, 0xFB, 0x07, 0x94, 0xC8, 0x1C, 0xD3, 0x88, 0x80, 0x71, 0x01, 0x98, 0xAC, 0xB9, 0x13, 0xB9};

        Result GenerateMac(u8 *out, u8 *in, size_t size, const u8 kek[0x10], const u8 key_source[0x10]) {
            u32 keyslot;
            R_TRY(splCryptoLockAesEngine(&keyslot));
            ON_SCOPE_EXIT {
                splCryptoUnlockAesEngine(keyslot);
            };
            R_TRY(splCryptoLoadAesKey(kek, key_source, keyslot));
            R_TRY(splCryptoComputeCmac(in, size, keyslot, out));
            return ResultSuccess();
        }

    }

    Result Initialize() {
        R_TRY(splCryptoGenerateAesKek(screenshot_kek_source, 0, 0, screenshot_kek));
        R_TRY(splCryptoGenerateAesKek(movie_kek_source, 0, 0, movie_kek));
        u8 aes_kek[0x10] = {0};
        u8 aes_aes_key[0x10] = {0};
        R_TRY(splCryptoGenerateAesKek(aes_kek_source, 0, 0, aes_kek));
        R_TRY(splCryptoGenerateAesKey(aes_kek, aes_key_source, aes_aes_key));

        Aes128Context ctx;
        aes128ContextCreate(&ctx, aes_aes_key, 0);
        aes128DecryptBlock(&ctx, aes128::key, aes128::encrypted_key);
        aes128DecryptBlock(&ctx, aes256::key, aes256::encrypted_key);
        aes128DecryptBlock(&ctx, aes256::key + 0x10, aes256::encrypted_key + 0x10);

        return ResultSuccess();
    }

    Result GenerateScreenshotMac(u8 *out, u8 *in, size_t size) {
        return GenerateMac(out, in, size, screenshot_kek, screenshot_key_source);
    }

    Result GenerateMovieMac(u8 *out, u8 *in, size_t size) {
        return GenerateMac(out, in, size, movie_kek, movie_key_source);
    }

    namespace aes128 {

        void Decrypt(u64 *dst, const u64 *src) {
            Aes128Context ctx;
            aes128ContextCreate(&ctx, key, false);
            aes128DecryptBlock(&ctx, dst, src);
        }

        void Encrypt(u64 *dst, const u64 *src) {
            Aes128Context ctx;
            aes128ContextCreate(&ctx, key, true);
            aes128EncryptBlock(&ctx, dst, src);
        }

    }

    namespace aes256 {

        void EncryptV0(ApplicationEntry *out, const Entry *src, const u8 v0Key[0x20]) {
            Aes256Context ctx;
            aes256ContextCreate(&ctx, v0Key, 1);
            aes256EncryptBlock(&ctx, out->data, ((u8 *)src));
            aes256EncryptBlock(&ctx, out->data + 0x10, ((u8 *)src) + 0x10);
        }

        Result EncryptV1(ApplicationEntry *out, const Entry *src, u64 version) {
            R_UNLESS(version == 1, 0x800ce);
            u64 tmp[4];
            Entry tmpEntry = *src;
            tmpEntry.fileId.pad[0x5] = 1;
            Aes256Context ctx;
            aes256ContextCreate(&ctx, key, 1);
            aes256EncryptBlock(&ctx, tmp, ((u8 *)&tmpEntry));
            aes256EncryptBlock(&ctx, tmp + 2, ((u8 *)&tmpEntry) + 0x10);
            out->v1.size = src->size;
            out->v1.hash = tmp[0] ^ tmp[1] ^ tmp[2] ^ tmp[3];
            out->v1.datetime = src->fileId.datetime;
            out->v1.storage = src->fileId.storage;
            out->v1.content = src->fileId.type;
            out->v1.unk_x1f = 1;
            return ResultSuccess();
        }

    }

}