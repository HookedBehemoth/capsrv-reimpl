#include "capsrv_crypto.hpp"

namespace ams::capsrv::crypto {

u8 screenshot_kek_source[0x10] = {0x9b, 0xbf, 0xde, 0x01, 0xfa, 0x18, 0xbb, 0xee, 0x87, 0xa4, 0xac, 0xc5, 0x67, 0xac, 0xd4, 0x21};
u8 screenshot_key_source[0x10] = {0x15, 0x0d, 0x26, 0xd1, 0x17, 0xee, 0x44, 0xff, 0x79, 0x67, 0x43, 0x8a, 0x20, 0x14, 0x63, 0x30};
u8 screenshot_kek[0x10] = {0};

u8 movie_kek_source[0x10] = {0x55, 0xb4, 0x30, 0xaf, 0x16, 0x8d, 0x9a, 0x66, 0x6a, 0x8f, 0xed, 0xe3, 0xa5, 0x52, 0x32, 0xea};
u8 movie_key_source[0x10] = {0x71, 0x70, 0x7d, 0xed, 0x3e, 0x2f, 0x48, 0x03, 0x37, 0x2f, 0x42, 0x35, 0x12, 0x39, 0x2c, 0xfe};
u8 movie_kek[0x10] = {0};

Result Initialize() {
	R_TRY(splCryptoGenerateAesKek(screenshot_kek_source, 0, 0, screenshot_kek));
	R_TRY(splCryptoGenerateAesKek(movie_kek_source, 0, 0, movie_kek));
	return ResultSuccess();
}

Result GenerateScreenshotMac(u8 *out, u8 *in, size_t size) {
	u32 keyslot;
	R_TRY(splCryptoLockAesEngine(&keyslot));
	ON_SCOPE_EXIT {
		splCryptoUnlockAesEngine(keyslot);
	};
	R_TRY(splCryptoLoadAesKey(screenshot_kek, screenshot_key_source, keyslot));
	R_TRY(splCryptoComputeCmac(in, size, keyslot, out));
	return ResultSuccess();
}

Result GenerateMovieMac(u8 *out, u8 *in, size_t size) {
	u32 keyslot;
	R_TRY(splCryptoLockAesEngine(&keyslot));
	ON_SCOPE_EXIT {
		splCryptoUnlockAesEngine(keyslot);
	};
	R_TRY(splCryptoLoadAesKey(movie_kek, movie_key_source, keyslot));
	R_TRY(splCryptoComputeCmac(in, size, keyslot, out));
	return ResultSuccess();
}

namespace aes128 {

// TODO: seal actual key
u8 key[0x10] =
	{
		0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

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

/*Result DecryptApplicationId(u64 *applicationId, u8 *src) {
	Aes128Context ctx;
	u64 tmp[2];
	aes128ContextCreate(&ctx, key, false);
	aes128DecryptBlock(&ctx, tmp, src);

	R_UNLESS(tmp[1] == 0, 0x16ce);
	*applicationId = tmp[0];
	return ResultSuccess();
}

void EncryptApplicationId(u64 *dst, u64 applicationId) {
	Aes128Context ctx;
	u64 tmp[2];
	tmp[0] = applicationId;
	aes128ContextCreate(&ctx, key, true);
	aes128EncryptBlock(&ctx, dst, tmp);
}*/

} // namespace aes128

} // namespace ams::capsrv::crypto