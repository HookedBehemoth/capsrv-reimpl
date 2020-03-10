#pragma once
#include <stratosphere.hpp>
#include <string>
#include <type_traits>

#include "capsrv_results.hpp"
#include "fs/fs_image_directory.hpp"

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

        const char *AsString() const;
    };
    static_assert(sizeof(DateTime) == sizeof(CapsAlbumFileDateTime));

    struct FileId {
        u64 applicationId;
        DateTime datetime;
        StorageId storage;
        ContentType type;
        u8 pad[0x6];

        const char *AsString() const;
        u64 GetFolderPath(char *buffer, u64 max_length) const;
        u64 GetFileName(char *buffer, u64 max_length) const;
        u64 GetFilePath(char *buffer, u64 max_length) const;

        inline bool IsExtra() const {
            return (u8)this->type > 1;
        }
        inline bool IsNormal() const {
            return !IsExtra();
        }

        Result Verify() const;

        static Result FromString(FileId *fileId, StorageId storage, const char *str);
        inline bool operator==(const FileId &f) {
            //return (this->applicationId == f.applicationId) && (this->storage == f.storage) && (this->type == f.type);
            return memcmp(this, &f, sizeof(FileId)) == 0;
        }
        inline bool operator!=(const FileId &f) {
            return !this->operator==(f);
        }
    };
    static_assert(sizeof(FileId) == sizeof(CapsAlbumFileId));

    struct Entry {
        s64 size;
        FileId fileId;

        const char *AsString() const;
        inline bool operator==(const Entry &f) {
            return memcmp(this, &f, sizeof(FileId)) == 0;
        }
        inline bool operator!=(const Entry &f) {
            return !this->operator==(f);
        }
    };
    static_assert(sizeof(Entry) == sizeof(CapsAlbumEntry));

    struct ApplicationEntry {
        union {
            u8 data[0x20];

            struct {
                u8 unk_x0[0x20];
            } v0;

            struct {
                s64 size;
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

    struct ApplicationFileEntry {
        ApplicationEntry entry;
        DateTime datetime;
        u64 unk_x28;
    };
    static_assert(sizeof(ApplicationFileEntry) == sizeof(CapsApplicationAlbumFileEntry));

    struct Dimensions {
        u64 width;
        u64 height;
    };

    struct ContentStorage {
        CapsAlbumCache cache[2][4]{};

        Result CanSave(StorageId storage, ContentType type) const;
        void Set(StorageId storage, ContentType type, CapsAlbumCache cache);
        void Increment(StorageId storage, ContentType type);
        void Decrement(StorageId storage, ContentType type);
    };

    struct AlbumUsage16 : sf::LargeData, sf::PrefersMapAliasTransferMode, CapsAlbumUsage16 {};
    struct LoadAlbumScreenShotImageOutput : sf::LargeData, sf::PrefersMapAliasTransferMode, CapsLoadAlbumScreenShotImageOutput {};
    struct LoadAlbumScreenShotImageOutputForApplication : sf::LargeData, sf::PrefersMapAliasTransferMode, CapsLoadAlbumScreenShotImageOutputForApplication {};

}