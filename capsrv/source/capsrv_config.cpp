#include "capsrv_config.hpp"

#include "impl/capsrv_manager.hpp"

namespace ams::capsrv::config {

    namespace {

        constexpr s64 maxFileSize[2][2] = {
            [StorageId::Nand] = {
                [ContentType::Screenshot] = 0x7D000,
                [ContentType::Movie] = 0x80000000,
            },
            [StorageId::Sd] = {
                [ContentType::Screenshot] = 0x7D000,
                [ContentType::Movie] = 0x80000000,
            },
        };

        bool defaultDirectory = true;
        //bool debugMode = false;
        bool screenshotSupport = true;
        bool movieSupport = false;
        bool verifyScreenShotFiledata = true;
        bool verifyMovieFileSignature = true;
        bool verifyMovieFileHash = true;
        u64 nandScreenshotMax = 1000;
        u64 nandMovieMax = 100;
        u64 sdScreenshotMax = 10000;
        u64 sdMovieMax = 1000;
        char *directoryPath = nullptr;

    }

    void Initialize() {

        /* TODO: Debug mode implementation with TMA. */
        //debugMode = settings::fwdbg::IsDebugModeEnabled();
        //defaultDirectory = !debugMode;

        settings::fwdbg::GetSettingsItemValue(&verifyScreenShotFiledata, 1, "capsrv", "enable_album_screenshot_filedata_verification");

        if (hosversionBefore(4, 0, 0))
            return;

        settings::fwdbg::GetSettingsItemValue(&screenshotSupport, 1, "capsrv", "enable_album_screenshot_file_support");
        settings::fwdbg::GetSettingsItemValue(&movieSupport, 1, "capsrv", "enable_album_movie_file_support");
        settings::fwdbg::GetSettingsItemValue(&verifyMovieFileSignature, 1, "capsrv", "enable_album_movie_filesign_verification");
        settings::fwdbg::GetSettingsItemValue(&verifyMovieFileHash, 1, "capsrv", "enable_album_movie_filehash_verification");

        if (hosversionBefore(5, 0, 0))
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

    s64 GetMaxFileSize(StorageId storage, ContentType type) {
        return maxFileSize[storage][type];
    }

    const char *GetCustomDirectoryPath() {
        if (defaultDirectory)
            return nullptr;
        return directoryPath;
    }

}