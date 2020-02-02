#pragma once
#include <stratosphere.hpp>

#include <string>
#include <type_traits>

namespace ams::capsrv {

    /* Storage IDs. */
    enum StorageId : u8 {
        Nand    = 0,
        Sd      = 1,
    };

    /* Content IDs. */
    enum ContentType : u8 {
        Screenshot      = 0,
        Movie           = 1,
        ExtraScreenshot = 2,
        ExtraMovie      = 3,
    };

    struct DateTime : CapsAlbumFileDateTime {
        std::string AsString() const {
            char out[36];
            snprintf(out, 36, "[%04d:%02d:%02d %02d:%02d:%02d %02d]", this->year, this->month, this->day, this->hour, this->minute, this->second, this->id);
            return std::string(out);
        }
    };

    struct FileId {
        u64 applicationId;
        DateTime datetime;
        StorageId storage;
        ContentType type;
        std::string AsString() const {
            char out[60];
            snprintf(out, 60, "%016lx, %s, %hhd, %hhd", this->applicationId, this->datetime.AsString().c_str(), this->storage, this->type);
            return std::string(out);
        }
    };

    struct ApplicationFileId {
        u64 unk_x0;
        u64 unk_x8;
        DateTime datetime;
        u64 unk_x18;
        std::string AsString() const {
            char out[120];
            snprintf(out, 120, "%016lx, %016lx, %s, %016lx", this->unk_x0, this->unk_x8, this->datetime.AsString().c_str(), this->unk_x18);
            return std::string(out);
        }
    };

    struct ApplicationEntry {
        ApplicationFileId fileId;
        DateTime datetime;
        u64 unk_x28;
        std::string AsString() const {
            char out[200];
            snprintf(out, 200, "%s, %s, %016lx", this->fileId.AsString().c_str(), this->datetime.AsString().c_str(), this->unk_x28);
            return std::string(out);
        }
    };

    struct Dimensions {
        u64 width;
        u64 height;
    };

    struct AlbumUsage16 : sf::LargeData, sf::PrefersMapAliasTransferMode, CapsAlbumUsage16 {};
    struct LoadAlbumScreenShotImageOutput : sf::LargeData, sf::PrefersMapAliasTransferMode, CapsLoadAlbumScreenShotImageOutput {};
    struct LoadAlbumScreenShotImageOutputForApplication : sf::LargeData, sf::PrefersMapAliasTransferMode, CapsLoadAlbumScreenShotImageOutputForApplication {};

}