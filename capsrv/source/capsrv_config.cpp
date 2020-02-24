#include "capsrv_config.hpp"

#include "impl/capsrv_fs.hpp"

#define CAPSRV_SET_GET_BOOL(var, key)                                \
	rc = setsysGetSettingsItemValue("capsrv", key, &temp, 1, &size); \
	if (R_SUCCEEDED(rc))                                             \
		var = temp;

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
	nandScreenshotMax = 1000;
	nandMovieMax = 100;
	sdScreenshotMax = 10000;
	sdMovieMax = 1000;

	// TODO: Debug mode implementation
	/*bool debug = settings::fwdbg::IsDebugModeEnabled();
	debugMode = debug;
	defaultDirectory = !debug;*/
	debugMode = false;
	defaultDirectory = true;

	settings::fwdbg::GetSettingsItemValue(&verifyScreenShotFiledata, 1, "capsrv", "enable_album_screenshot_filedata_verification");

	if (hosversionBefore(4,0,0))
		return;

	settings::fwdbg::GetSettingsItemValue(&screenshotSupport, 1, "capsrv", "enable_album_screenshot_file_support");
	settings::fwdbg::GetSettingsItemValue(&movieSupport, 1, "capsrv", "enable_album_movie_file_support");
	settings::fwdbg::GetSettingsItemValue(&verifyMovieFileSignature, 1, "capsrv", "enable_album_movie_filesign_verification");
	settings::fwdbg::GetSettingsItemValue(&verifyMovieFileHash, 1, "capsrv", "enable_album_movie_filehash_verification");

	if (hosversionBefore(5,0,0))
		return;

	bool changeDirectory = false;
	settings::fwdbg::GetSettingsItemValue(&changeDirectory, 1, "capsrv", "enable_album_directory_change");
	if (changeDirectory) {
		u64 size = 0;
		char *tempStr = new char[0x100];
		size = settings::fwdbg::GetSettingsItemValue(tempStr, sizeof(tempStr), "capsrv", "album_directory_path");
		if (size <= 0x100) {
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
			return screenshotSupport;
		case ContentType::Movie:
			return movieSupport;
		case ContentType::ExtraScreenshot:
		case ContentType::ExtraMovie:
			return true;
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

void print() {
	printf(R"(defaultDirectory: %d,
debugMode: %d,
screenshotSupport: %d,
movieSupport: %d,
verifyScreenShotFiledata: %d,
verifyMovieFileSignature: %d,
verifyMovieFileHash: %d,
nandScreenshotMax: %ld,
nandMovieMax: %ld,
sdScreenshotMax: %ld,
sdMovieMax: %ld,
directoryPath: %s
)",
		   defaultDirectory,
		   debugMode,
		   screenshotSupport,
		   movieSupport,
		   verifyScreenShotFiledata,
		   verifyMovieFileSignature,
		   verifyMovieFileHash,
		   nandScreenshotMax,
		   nandMovieMax,
		   sdScreenshotMax,
		   sdMovieMax,
		   directoryPath);
}

} // namespace ams::capsrv::config