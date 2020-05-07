#pragma once

#include <stratosphere.hpp>

#include "capsrv_content_type.hpp"
#include "capsrv_storage.hpp"

namespace ams::capsrv {

    class AlbumSettings {
      private:
        bool defaultDirectory;
        bool debugMode;

        bool screenshotSupport;
        bool movieSupport;

        bool verifyScreenShotFiledata;
        bool verifyMovieFileSignature;
        bool verifyMovieFileHash;

        u32 unused;

        u64 nandScreenshotMaxCount;
        u64 nandMovieMaxCount;
        u64 sdScreenshotMaxCount;
        u64 sdMovieMaxCount;
        char *custom_directory_path;

      public:
        AlbumSettings();
        void Initialize();
        void Exit();

        bool SupportsScreenshot();
        bool SupportsMovie();
        bool SupportsExtraScreenshot();
        bool SupportsExtraMovie();

        bool AllowSaveExtraMoviesOnNand();
        bool IsTypeSupported(ContentType type);
        bool GetVerifyScreenshotFiledata();
        bool SetVerifyScreenshotFiledata(bool new_value);
        bool GetVerifyMovieSignature();
        bool GetVerifyMovieHash();

        size_t GetFileMaxCount(StorageId storage, ContentType type);
        const char *GetCustomImageDirectoryPath();
    };

    s64 GetAllowedFileSize(ContentType type, AlbumSettings *settings);
    Result VerifyContentType(ContentType type, AlbumSettings *settings);

    Result MountAlbum(StorageId storage_id, AlbumSettings *settings);
    Result UnmountAlbum(StorageId storage_id, AlbumSettings *settings);

}
