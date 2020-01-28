#include "capsrv_settings.hpp"

//#include <stratosphere.hpp>
#include <switch.h>

#define CAPSRV_SET_GET_BOOL(var, key)\
rc = setsysGetSettingsItemValue("capsrv", key, &temp, 1, &size);\
if (R_SUCCEEDED(rc) && size == 1) var = temp || rc == 0;

namespace ams::capsrv {

    Settings::Settings() {
        this->defaultDirectory = true;
        this->isDebug = false;
        this->screenshotSupport = false;
        this->movieSupport = false;
        this->screenshotFiledataVerification = true;
        this->movieFilesignVerification = true;
        this->movieFilehashVerification = true;
        this->nandScreenshotMax = 0;
        this->nandMovieMax = 0;
        this->sdScreenshotMax = 0;
        this->sdMovieMax = 0;
        this->directoryPath = nullptr;
    }

    Settings::~Settings() {
        if (this->directoryPath != nullptr)
            delete this->directoryPath;
    }

    void Settings::Initialize() {
        u64 size = 0;
        bool temp = false;
        Result rc = 0;

        this->nandScreenshotMax = 1000;
        this->nandMovieMax = 100;
        this->sdScreenshotMax = 10000;
        this->sdMovieMax = 1000;

        CAPSRV_SET_GET_BOOL(this->screenshotSupport, "enable_album_screenshot_file_support");
        CAPSRV_SET_GET_BOOL(this->movieSupport, "enable_album_movie_file_support");
        CAPSRV_SET_GET_BOOL(this->screenshotFiledataVerification, "enable_album_screenshot_filedata_verification");
        CAPSRV_SET_GET_BOOL(this->movieFilesignVerification, "enable_album_movie_filesign_verification");
        CAPSRV_SET_GET_BOOL(this->movieFilehashVerification, "enable_album_movie_filehash_verification");
        bool changeDirectory = false;
        CAPSRV_SET_GET_BOOL(changeDirectory, "enable_album_directory_change");
        if (changeDirectory) {
            char *tempStr = new char[0x100];
            rc = setsysGetSettingsItemValue("capsrv", "album_directory_path", tempStr, sizeof(tempStr), &size);
            if (R_SUCCEEDED(rc) && size < 0x101) {
                this->directoryPath = tempStr;
                return;
            }
            delete tempStr;
        }
    }

    bool Settings::IsDefaultDirectory() {
        return this->defaultDirectory;
    }

    bool Settings::IsDebug() {
        return this->isDebug;
    }

    bool Settings::SupportsScreenshot() {
        return this->screenshotSupport;
    }

    bool Settings::SupportsMovie() {
        return this->movieSupport;
    }

    bool Settings::VerifyScreenshotFiledata() {
        return this->screenshotFiledataVerification;
    }

    bool Settings::VerifyMovieFilesign() {
        return this->movieFilesignVerification;
    }

    bool Settings::VerifyMovieFilehash() {
        return this->movieFilehashVerification;
    }

    u64 Settings::GetNandScreenshotMax() {
        return this->nandScreenshotMax;
    }
    
    u64 Settings::GetNandMovieMax() {
        return this->nandMovieMax;
    }

    u64 Settings::GetSdScreenshotMax() {
        return this->sdScreenshotMax;
    }

    u64 Settings::GetSdMovieMax() {
        return this->sdMovieMax;
    }

    std::string Settings::GetCustomDirectoryPath() {
        return std::string(this->directoryPath);
    }

}