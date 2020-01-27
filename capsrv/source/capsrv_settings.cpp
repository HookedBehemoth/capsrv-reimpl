#include "capsrv_settings.hpp"

//#include <stratosphere.hpp>
#include <switch.h>

namespace ams::capsrv {

    Settings::Settings() {
        u64 tmp;
        setsysGetSettingsItemValue("capsrv", "enable_album_screenshot_file_support", &this->screenshotSupport, 1, &tmp);
    }

    Settings::~Settings() {

    }


}