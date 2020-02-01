#pragma once
#include <stratosphere.hpp>
#include "capsrv_types.hpp"

#include <string>

namespace ams::capsrv {
    class Settings {
        private:
            bool defaultDirectory;
            bool debugMode;
            bool screenshotSupport;
            bool movieSupport;
            bool verifyScreenShotFiledata;
            bool verifyMovieFileSignature;
            bool verifyMovieFileHash;
            u64 nandScreenshotMax;
            u64 nandMovieMax;
            u64 sdScreenshotMax;
            u64 sdMovieMax;
            char *directoryPath;
        public:
            Settings();
            ~Settings();
            void Initialize();

            bool SupportsType(ContentType type) const;
            bool StorageValid(StorageId storage) const;
            inline u64 GetMax(StorageId storage, ContentType type) const;
            const char* GetCustomDirectoryPath() const;

            Result MountAlbum(StorageId storage);
            Result UnmountAlbum(StorageId storage);
    };
}