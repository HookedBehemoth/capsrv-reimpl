#include "capsrv_settings.hpp"
#include "capsrv_fs.hpp"

#define CAPSRV_SET_GET_BOOL(var, key)\
rc = setsysGetSettingsItemValue("capsrv", key, &temp, 1, &size);\
if (R_SUCCEEDED(rc) && size == 1) var = temp || rc.IsSuccess();

namespace ams::capsrv {

    Settings::Settings() {
        this->defaultDirectory = true;
        this->debugMode = false;
        this->screenshotSupport = false;
        this->movieSupport = false;
        this->verifyScreenShotFiledata = true;
        this->verifyMovieFileSignature = true;
        this->verifyMovieFileHash = true;
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

        bool debug = settings::fwdbg::IsDebugModeEnabled();
        this->debugMode = debug;
        this->defaultDirectory = !debug;

        CAPSRV_SET_GET_BOOL(this->screenshotSupport, "enable_album_screenshot_file_support");
        CAPSRV_SET_GET_BOOL(this->movieSupport, "enable_album_movie_file_support");
        CAPSRV_SET_GET_BOOL(this->verifyScreenShotFiledata, "enable_album_screenshot_filedata_verification");
        CAPSRV_SET_GET_BOOL(this->verifyMovieFileSignature, "enable_album_movie_filesign_verification");
        CAPSRV_SET_GET_BOOL(this->verifyMovieFileHash, "enable_album_movie_filehash_verification");
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

    bool Settings::SupportsType(const ContentType type) const {
        switch (type) {
            case ContentType::Screenshot:
            case ContentType::ExtraScreenshot:
                return this->screenshotSupport;
            case ContentType::Movie:
            case ContentType::ExtraMovie:
                return this->movieSupport;
        }
        return false;
    }

    bool Settings::StorageValid(const StorageId storage) const {
        return storage == StorageId::Nand || storage == StorageId::Sd;
    }
    
    u64 Settings::GetMax(const StorageId storage, const ContentType type) const {
        switch (storage) {
            case StorageId::Nand:
                switch (type) {
                    case ContentType::Screenshot:
                    case ContentType::ExtraScreenshot:
                        return this->nandScreenshotMax;
                    case ContentType::Movie:
                    case ContentType::ExtraMovie:
                        return this->nandMovieMax;
                }
                break;
            case StorageId::Sd:
                switch (type) {
                    case ContentType::Screenshot:
                    case ContentType::ExtraScreenshot:
                        return this->sdScreenshotMax;
                    case ContentType::Movie:
                    case ContentType::ExtraMovie:
                        return this->sdMovieMax;
                }
        }
        return 0;
    }

    const char* Settings::GetCustomDirectoryPath() const {
        if (this->defaultDirectory)
            return nullptr;
        return this->directoryPath;
    }

    Result Settings::MountAlbum(const StorageId storage) {
        if (storage != StorageId::Nand && storage != StorageId::Sd)
            return capsrv::ResultInvalidStorageId();

        const char* customDirectory = this->GetCustomDirectoryPath();
        if (storage == StorageId::Sd && customDirectory) {
            return MountCustomImageDirectory(customDirectory);
        }
        return MountImageDirectory(storage);
    }

    Result Settings::UnmountAlbum(const StorageId storage) {
        if (storage != StorageId::Nand && storage != StorageId::Sd)
            return capsrv::ResultInvalidStorageId();

        return UnmountImageDirectory(storage);
    }

}