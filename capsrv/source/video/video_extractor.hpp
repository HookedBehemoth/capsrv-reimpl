#pragma once

#include <vapours.hpp>

namespace ams {

    Result GetOffsets(u64 *jpegOffset, u64 *jpegSize, u64 *macOffset, u64 *macSize, u64 mpegSize, u64 mpegTail);

}
