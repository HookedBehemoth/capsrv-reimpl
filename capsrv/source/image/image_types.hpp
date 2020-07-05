#pragma once

namespace ams::image {

    enum JpegStatus {
        JpegStatus_Ok                = 0,
        JpegStatus_InvalidFormat     = -0x20,
        JpegStatus_UnsupportedFormat = -0x21,
        JpegStatus_OutOfMemory       = -0x40,
    };

    enum ProcessStage {
        ProcessStage_Unregistered = 0,
        ProcessStage_Registered   = 1,
        ProcessStage_Analyzed     = 2,
    };

    struct Dimension {
        u32 width;
        u32 height;
    };

    enum class ExifOrientation : u16 {
        UpperLeft  = 1,
        LowerRight = 3,
        UpperRight = 6,
        LowerLeft  = 8,
        Undefined  = 9,
    };

}
