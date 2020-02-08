#include "capsrv_config.hpp"

#include "impl/capsrv_fs.hpp"

#define CAPSRV_SET_GET_BOOL(var, key)                                \
	rc = setsysGetSettingsItemValue("capsrv", key, &temp, 1, &size); \
	if (R_SUCCEEDED(rc))                                             \
		var = temp || rc.IsSuccess();

namespace ams::capsrv::config {

namespace {
bool defaultDirectory = true;
bool debugMode = false;
bool screenshotSupport = false;
bool movieSupport = false;
bool verifyScreenShotFiledata = true;
bool verifyMovieFileSignature = true;
bool verifyMovieFileHash = true;
u64 nandScreenshotMax = 0;
u64 nandMovieMax = 0;
u64 sdScreenshotMax = 0;
u64 sdMovieMax = 0;
char *directoryPath = nullptr;
} // namespace

void Initialize() {
	u64 size = 0;
	bool temp = false;
	Result rc = 0;

	nandScreenshotMax = 1000;
	nandMovieMax = 100;
	sdScreenshotMax = 10000;
	sdMovieMax = 1000;

	bool debug = settings::fwdbg::IsDebugModeEnabled();
	debugMode = debug;
	defaultDirectory = !debug;

	CAPSRV_SET_GET_BOOL(screenshotSupport, "enable_album_screenshot_file_support");
	CAPSRV_SET_GET_BOOL(movieSupport, "enable_album_movie_file_support");
	CAPSRV_SET_GET_BOOL(verifyScreenShotFiledata, "enable_album_screenshot_filedata_verification");
	CAPSRV_SET_GET_BOOL(verifyMovieFileSignature, "enable_album_movie_filesign_verification");
	CAPSRV_SET_GET_BOOL(verifyMovieFileHash, "enable_album_movie_filehash_verification");
	bool changeDirectory = false;
	CAPSRV_SET_GET_BOOL(changeDirectory, "enable_album_directory_change");
	if (changeDirectory) {
		char *tempStr = new char[0x100];
		rc = setsysGetSettingsItemValue("capsrv", "album_directory_path", tempStr, sizeof(tempStr), &size);
		if (R_SUCCEEDED(rc) && size < 0x101) {
			directoryPath = tempStr;
			return;
		}
		delete tempStr;
	}
}

void Exit() {
	if (directoryPath != nullptr)
		delete directoryPath;
}

bool SupportsType(ContentType type) {
	switch (type) {
		case ContentType::Screenshot:
		case ContentType::ExtraScreenshot:
			return screenshotSupport;
		case ContentType::Movie:
		case ContentType::ExtraMovie:
			return movieSupport;
	}
	return false;
}

bool StorageValid(StorageId storage) {
	return storage == StorageId::Nand || storage == StorageId::Sd;
}

u64 GetMax(StorageId storage, ContentType type) {
	switch (storage) {
		case StorageId::Nand:
			switch (type) {
				case ContentType::Screenshot:
				case ContentType::ExtraScreenshot:
					return nandScreenshotMax;
				case ContentType::Movie:
				case ContentType::ExtraMovie:
					return nandMovieMax;
			}
			break;
		case StorageId::Sd:
			switch (type) {
				case ContentType::Screenshot:
				case ContentType::ExtraScreenshot:
					return sdScreenshotMax;
				case ContentType::Movie:
				case ContentType::ExtraMovie:
					return sdMovieMax;
			}
	}
	return 0;
}

const char *GetCustomDirectoryPath() {
	if (defaultDirectory)
		return nullptr;
	return directoryPath;
}

} // namespace ams::capsrv::config