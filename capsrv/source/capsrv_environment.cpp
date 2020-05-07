#include "capsrv_environment.hpp"

namespace ams::capsrv {
    
    namespace {
        
        bool is_quest = false;
        bool is_debug = false;

    }

    void LoadEnvironment() {
        is_quest = ams::settings::fwdbg::IsQuest();
        is_debug = ams::settings::fwdbg::IsDebugModeEnabled();
    }

    bool IsQuest() {
        return is_quest;
    }

    bool IsDebug() {
        return is_debug;
    }

}
