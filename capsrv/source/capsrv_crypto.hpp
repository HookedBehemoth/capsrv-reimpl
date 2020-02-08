#pragma once
#include <stratosphere.hpp>

namespace ams::capsrv::crypto {

Result Initialize();

Result GenerateScreenshotMac(u8 *out, u8 *in, size_t size);
Result GenerateMovieMac(u8 *out, u8 *in, size_t size);

namespace aes128 {

void Decrypt(u64 *dst, const u64 *src);
void Encrypt(u64 *dst, const u64 *src);

} // namespace aes128

} // namespace ams::capsrv::crypto