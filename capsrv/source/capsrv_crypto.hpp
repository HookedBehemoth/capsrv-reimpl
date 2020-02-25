#pragma once
#include <stratosphere.hpp>

#include "capsrv_types.hpp"

namespace ams::capsrv::crypto {

    Result Initialize();

    Result GenerateScreenshotMac(u8 *out, u8 *in, size_t size);
    Result GenerateMovieMac(u8 *out, u8 *in, size_t size);

    namespace aes128 {

        void Decrypt(u64 *dst, const u64 *src);
        void Encrypt(u64 *dst, const u64 *src);

    }

    namespace aes256 {

        void EncryptV0(ApplicationEntry *out, const Entry *src, const u8 v0Key[0x20]);
        void DecryptV0(Entry *out, const ApplicationEntry *src, const u8 v0Key[0x20]);

        Result EncryptV1(ApplicationEntry *out, const Entry *src, u64 version);
        Result DecryptV1(Entry *out, const ApplicationEntry *src, u64 applicationId);

    }

}