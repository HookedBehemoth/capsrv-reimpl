#pragma once
#include <string>
#include <switch/types.h>

namespace ams::capsrv {
    class Settings {
        private:
            bool defaultDirectory;
            bool isDebug;
            bool screenshotSupport;
            bool movieSupport;
            bool screenshotFiledataVerification;
            bool movieFilesignVerification;
            bool movieFilehashVerification;
            u64 nandScreenshotMax;
            u64 nandMovieMax;
            u64 sdScreenshotMax;
            u64 sdMovieMax;
            char *directoryPath;
        public:
            Settings();
            ~Settings();
            void Initialize();

            bool IsDefaultDirectory();
            bool IsDebug();
            bool SupportsScreenshot();
            bool SupportsMovie();
            bool VerifyScreenshotFiledata();
            bool VerifyMovieFilesign();
            bool VerifyMovieFilehash();
            u64 GetNandScreenshotMax();
            u64 GetNandMovieMax();
            u64 GetSdScreenshotMax();
            u64 GetSdMovieMax();
            std::string GetCustomDirectoryPath();
    };
}