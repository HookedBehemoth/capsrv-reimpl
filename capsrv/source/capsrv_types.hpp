#pragma once
#include <stratosphere.hpp>
#include <string>
#include <type_traits>

namespace ams::capsrv {

    /* Storage IDs. */
    enum StorageId : u8 {
        Nand = 0,
        Sd = 1,
    };

    /* Content IDs. */
    enum ContentType : u8 {
        Screenshot = 0,
        Movie = 1,
        ExtraScreenshot = 2,
        ExtraMovie = 3,
    };

    struct DateTime {
        u16 year;
        u8 month;
        u8 day;
        u8 hour;
        u8 minute;
        u8 second;
        u8 id;

        std::string AsString() const;

        static Result FromString(DateTime *date, const char *str, const char **next);
    };
    static_assert(sizeof(DateTime) == sizeof(CapsAlbumFileDateTime));

    struct FileId {
        u64 applicationId;
        DateTime datetime;
        StorageId storage;
        ContentType type;
        u8 pad[0x6];

        std::string AsString() const;
        std::string GetFolderPath() const;
        std::string GetFileName() const;
        std::string GetFilePath() const;

        Result Verify() const;

        static Result FromString(FileId *fileId, StorageId storage, const char *str);
    };
    static_assert(sizeof(FileId) == sizeof(CapsAlbumFileId));

    struct Entry {
        u64 size;
        FileId fileId;

        std::string AsString() const;
    };
    static_assert(sizeof(Entry) == sizeof(CapsAlbumEntry));

    struct ApplicationEntry {
        union {
            u8 data[0x20];

            struct {
                u8 unk_x0[0x20];
            } v0;

            struct {
                u64 size;
                u64 hash;
                DateTime datetime;
                u8 storage;
                u8 content;
                u8 pad_x1a[0x5];
                u8 unk_x1f;
            } v1;
        };
    };
    static_assert(sizeof(ApplicationEntry) == sizeof(CapsApplicationAlbumEntry));

    struct Dimensions {
        u64 width;
        u64 height;
    };

    struct Storage {
        CapsAlbumCache cache[4];
    };

    struct ContentStorage {
        Storage position[2] = {0};

        Result CanSave(StorageId storage, ContentType type) const;
        void Increment(StorageId storage, ContentType type);
        void Decrement(StorageId storage, ContentType type);
    };

    struct AlbumUsage16 : sf::LargeData, sf::PrefersMapAliasTransferMode, CapsAlbumUsage16 {};
    struct LoadAlbumScreenShotImageOutput : sf::LargeData, sf::PrefersMapAliasTransferMode, CapsLoadAlbumScreenShotImageOutput {};
    struct LoadAlbumScreenShotImageOutputForApplication : sf::LargeData, sf::PrefersMapAliasTransferMode, CapsLoadAlbumScreenShotImageOutputForApplication {};

}