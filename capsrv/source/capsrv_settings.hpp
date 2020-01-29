#pragma once
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

            inline bool SupportsType(const ContentType type) const;
            inline bool StorageValid(const StorageId storage) const;
            inline u64 GetMax(const StorageId storage, const ContentType type) const;
            const char* GetCustomDirectoryPath() const;

            Result MountAlbum(const StorageId storage);
            Result UnmountAlbum(const StorageId storage);
    };
}