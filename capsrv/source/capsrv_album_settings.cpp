#include "capsrv_album_settings.hpp"

#include "capsrv_content_type.hpp"

namespace ams::capsrv {

    namespace {

        constexpr const s64 MaxSizeArray[4] = {
            0x7D000, 0x80000000, 0x7D000, 0x80000000,
        };

    }

    AlbumSettings::AlbumSettings() {
        this->defaultDirectory = true;
        this->debugMode = false;
        this->verifyScreenShotFiledata = true;
        this->verifyMovieFileSignature = true;
        this->verifyMovieFileHash = true;
        this->unused = 0;
        this->nandScreenshotMaxCount = 0;
        this->nandMovieMaxCount = 0;
        this->sdScreenshotMaxCount = 0;
        this->sdMovieMaxCount = 0;
    }

    void AlbumSettings::Initialize() {
        bool is_debug_enabled = settings::fwdbg::IsDebugModeEnabled();
        this->defaultDirectory = !debugMode;
        this->debugMode = is_debug_enabled;

        size_t settings_value_length;

        bool temp_verifyScreenShotFiledata = false;
        settings_value_length = settings::fwdbg::GetSettingsItemValue(&temp_verifyScreenShotFiledata, 1, "capsrv", "enable_album_screenshot_filedata_verification");
        this->verifyScreenShotFiledata = settings_value_length == 0 || temp_verifyScreenShotFiledata;

        /* Disable movie support on firmwares lower than [4.0.0]. */
        if (hosversionBefore(4, 0, 0)) {
            this->screenshotSupport = true;
            this->movieSupport = false;
            this->verifyMovieFileSignature = false;
            this->verifyMovieFileHash = false;
            this->custom_directory_path = nullptr;
            return;
        }

        bool temp_screenshotSupport = false;
        settings_value_length = settings::fwdbg::GetSettingsItemValue(&temp_screenshotSupport, 1, "capsrv", "enable_album_screenshot_file_support");
        this->screenshotSupport = settings_value_length == 0 || temp_screenshotSupport;

        bool temp_movieSupport = false;
        settings_value_length = settings::fwdbg::GetSettingsItemValue(&temp_movieSupport, 1, "capsrv", "enable_album_movie_file_support");
        this->movieSupport = settings_value_length == 0 || temp_movieSupport;

        bool temp_verifyMovieFileSignature = false;
        settings_value_length = settings::fwdbg::GetSettingsItemValue(&temp_verifyMovieFileSignature, 1, "capsrv", "enable_album_movie_filesign_verification");
        this->verifyMovieFileSignature = settings_value_length == 0 || temp_verifyMovieFileSignature;

        bool temp_verifyMovieFileHash = false;
        settings_value_length = settings::fwdbg::GetSettingsItemValue(&temp_verifyMovieFileHash, 1, "capsrv", "enable_album_movie_filehash_verification");
        this->verifyMovieFileHash = settings_value_length == 0 || temp_verifyMovieFileHash;

        this->unused = 3;
        this->nandScreenshotMaxCount = 1000;
        this->nandMovieMaxCount = 100;
        this->sdScreenshotMaxCount = 10000;
        this->sdMovieMaxCount = 1000;

        if (hosversionBefore(5, 0, 0)) {
            this->custom_directory_path = nullptr;
            return;
        }

        bool changeDirectory = false;
        settings_value_length = settings::fwdbg::GetSettingsItemValue(&changeDirectory, 1, "capsrv", "enable_album_directory_change");
        if ((settings_value_length != 0) && changeDirectory) {
            char *tempStr = new char[0x100];
            settings_value_length = settings::fwdbg::GetSettingsItemValue(tempStr, sizeof(tempStr), "capsrv", "album_directory_path");
            if (settings_value_length <= 0x100) {
                custom_directory_path = tempStr;
                return;
            }
            delete[] tempStr;
        }
    }

    void AlbumSettings::Exit() {
        if (this->custom_directory_path != nullptr)
            delete[] this->custom_directory_path;
        this->custom_directory_path = nullptr;
    }

    bool AlbumSettings::SupportsScreenshot() {
        return this->screenshotSupport;
    }

    bool AlbumSettings::SupportsMovie() {
        return this->movieSupport;
    }

    bool AlbumSettings::SupportsExtraScreenshot() {
        return true;
    }

    bool AlbumSettings::SupportsExtraMovie() {
        return true;
    }

    bool AlbumSettings::AllowSaveExtraMoviesOnNand() {
        return false;
    }

    bool AlbumSettings::IsTypeSupported(ContentType type) {
        switch (type) {
            case ContentType::Screenshot:
                return this->screenshotSupport;
            case ContentType::Movie:
                return this->movieSupport;
            case ContentType::ExtraScreenshot:
                return true;
            case ContentType::ExtraMovie:
                return true;
            default:
                return false;
        }
    }

    bool AlbumSettings::GetVerifyScreenshotFiledata() {
        return this->verifyScreenShotFiledata;
    }

    bool AlbumSettings::SetVerifyScreenshotFiledata(bool new_value) {
        bool old_value = this->verifyScreenShotFiledata;
        /* Only allow changing that setting if on Host. */
        if (!this->defaultDirectory) {
            this->verifyScreenShotFiledata = new_value;
        }
        return old_value;
    }

    bool AlbumSettings::GetVerifyMovieSignature() {
        return this->verifyMovieFileSignature;
    }

    bool AlbumSettings::GetVerifyMovieHash() {
        return this->verifyMovieFileHash;
    }

    size_t AlbumSettings::GetFileMaxCount(StorageId storage, ContentType type) {
        switch (storage) {
            case StorageId::Nand:
                switch (type) {
                    case ContentType::Screenshot:
                    case ContentType::ExtraScreenshot:
                        return this->nandScreenshotMaxCount;
                    case ContentType::Movie:
                    case ContentType::ExtraMovie:
                        return this->nandMovieMaxCount;
                    default:
                        return 0;
                }
                break;
            case StorageId::Sd:
                switch (type) {
                    case ContentType::Screenshot:
                    case ContentType::ExtraScreenshot:
                        return this->sdScreenshotMaxCount;
                    case ContentType::Movie:
                    case ContentType::ExtraMovie:
                        return this->sdMovieMaxCount;
                    default:
                        return 0;
                }
                break;
            default:
                return 0;
        }
    }

    const char *AlbumSettings::GetCustomImageDirectoryPath() {
        if (this->defaultDirectory)
            return nullptr;
        return this->custom_directory_path;
    }

    s64 GetAllowedFileSize(ContentType type, AlbumSettings *settings) {
        if (settings->IsTypeSupported(type)) {
            return MaxSizeArray[static_cast<u8>(type)];
        }
        return 0;
    }

    Result VerifyContentType(ContentType type, AlbumSettings *settings) {
        R_SUCCEED_IF(settings->IsTypeSupported(type));
        return capsrv::ResultAlbumInvalidFileContents();
    }

    Result MountAlbum(StorageId storage_id, AlbumSettings *settings) {
        const char *cust_path = settings->GetCustomImageDirectoryPath();

        if ((storage_id == StorageId::Sd) && (cust_path != nullptr)) {
            AMS_ABORT("Host filesystem not implemented.");

            //R_ABORT_UNLESS(fs::MountHost("SD", cust_path));
            R_TRY_CATCH(capsrv::ResultAlbumStorageUnavailable()) {
                R_CONVERT(fs::ResultUnexpected,   capsrv::ResultAlbumUnexpected())
                R_CONVERT(fs::ResultPathNotFound, capsrv::ResultAlbumPathNotFound())
                R_CATCH_ALL() { /* ... */ }
            } R_END_TRY_CATCH_WITH_ABORT_UNLESS;
        } else {
            R_TRY(VerifyStorageId(storage_id));

            R_TRY_CATCH(fs::MountImageDirectory(MountNames[u8(storage_id)], fs::ImageDirectoryId(storage_id))) {
                R_CATCH(fs::ResultMountNameAlreadyExists, fs::ResultAllocationFailure) { AMS_ABORT(); }
                R_CONVERT(fs::ResultSdCardAccessFailed, capsrv::ResultAlbumStorageUnavailable())
                R_CONVERT(fs::ResultPathAlreadyExists,  capsrv::ResultAlbumPathUnavailable())
                R_CATCH_ALL() { /* ... */ }
            } R_END_TRY_CATCH;
        }

        AMS_ASSERT(false);
        return capsrv::ResultAlbumError();
    }

    Result UnmountAlbum(StorageId storage_id, AlbumSettings *settings) {
        const char *mount_name;

        const char *cust_path = settings->GetCustomImageDirectoryPath();
        if ((storage_id == StorageId::Sd) && (cust_path != nullptr)) {
            mount_name = "SD";
        } else {
            R_TRY(VerifyStorageId(storage_id));
            mount_name = MountNames[u8(storage_id)];
        }

        fs::Unmount(mount_name);
        return ResultSuccess();
    }

}
