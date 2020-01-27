#pragma once
#include <string>

namespace ams::capsrv {
    class Settings {
        private:
            bool screenshotSupport;
            bool movieSupport;
            bool screenshotFiledataVerification;
            bool movieFilesignVerification;
            bool movieFilehashVerification;
            bool changeDirectory;
            char directoryPath[0x100];
        public:
            Settings();
            ~Settings();
            bool SupportsScreenshot();
            bool SupportsMovie();
            bool VerifyScreenshotFiledata();
            bool VerifyMovieFilesign();
            bool VerifyMovieFilehash();
            bool CustomDirectory();
            std::string GetCustomDirectoryPath();
    };
}