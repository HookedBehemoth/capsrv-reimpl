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
    enum class ContentType : u8 {
        Screenshot      = 0,
        Movie           = 1,
        ExtraScreenshot = 2,
        ExtraMovie      = 3,
    };

    struct DateTime : CapsAlbumFileDateTime {
        std::string AsString() const {
            const char* tmp = "%04d:%02d:%02d %02d:%02d:%02d %02d";
            char out[36];
            snprintf(out, 36, tmp, this->year, this->month, this->day, this->hour, this->minute, this->second, this->id);
            return std::string(out);
        }
    };

    struct FileId {
        u64 applicationId;
        DateTime datetime;
        StorageId storage;
        ContentType type;
        std::string AsString() const {
            const char* tmp = "%016lx, %s, %hhd, %hhd";
            char out[60];
            snprintf(out, 36, tmp, this->applicationId, this->datetime.AsString().c_str(), this->storage, this->type);
            return std::string(out);
        }
    };

    struct ApplicationFileId {
        u64 unk_x0;
        u64 unk_x8;
        DateTime dateTime;
        u64 unk;
    };

    struct ApplicationFileEntry {
        ApplicationFileId fileId;
        DateTime datetime;
        u64 unk_x28;
    };

    struct ApplicationEntry : CapsApplicationAlbumEntry {
        std::string AsString() const {
            return ((DateTime*)&this->v1.datetime)->AsString();
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