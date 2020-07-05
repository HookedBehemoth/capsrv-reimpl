#pragma once

#include <stratosphere.hpp>

namespace ams::capsrv {

    enum class ContentType : u8 {
        Screenshot      = 0,
        Movie           = 1,
        ExtraScreenshot = 2,
        ExtraMovie      = 3,

        Count,
    };

    enum ContentTypeFlag {
        ContentTypeFlag_ScreenShot      = BIT(0),
        ContentTypeFlag_Movie           = BIT(1),
        ContentTypeFlag_ExtraScreenShot = BIT(2),
        ContentTypeFlag_ExtraMovie      = BIT(3),

        ContentTypeFlag_Normal  = ContentTypeFlag_ScreenShot | ContentTypeFlag_Movie,
        ContentTypeFlag_Extra   = ContentTypeFlag_ExtraScreenShot | ContentTypeFlag_ExtraMovie,
    };

    [[maybe_unused]]
    static const char *GetFileExtension(ContentType type) {
        switch (type) {
            case ContentType::Screenshot:
            case ContentType::ExtraScreenshot:
                return ".jpg";
            case ContentType::Movie:
            case ContentType::ExtraMovie:
                return ".mp4";
            default:
                return nullptr;
        }
    }

    /*static u8 GetContentCount() {
        return static_cast<u8>(ContentType::Count);
    }*/

    ALWAYS_INLINE bool ContentTypeValid(ContentType type) {
        return type == ContentType::Screenshot || type == ContentType::Movie || type == ContentType::ExtraScreenshot || type == ContentType::ExtraMovie;
    }

    ALWAYS_INLINE bool IsScreenShot(ContentType type) {
        return type == ContentType::Screenshot || type == ContentType::ExtraScreenshot;
    }

    ALWAYS_INLINE bool IsMovie(ContentType type) {
        return !IsScreenShot(type);
    }

    ALWAYS_INLINE bool IsNormal(ContentType type) {
        return type == ContentType::Screenshot || type == ContentType::Movie;
    }

    ALWAYS_INLINE bool IsExtra(ContentType type) {
        return !IsNormal(type);
    }

}
