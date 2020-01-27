#pragma once

#include <type_traits>
#include <switch/types.h>

namespace ams::capsrv {

    /* Storage IDs. */
    enum class StorageId : u8 {
        Nand    = 0,
        Sd      = 1,
    };
}