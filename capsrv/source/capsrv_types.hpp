#pragma once
#include <stratosphere.hpp>
#include <string>

#include "capsrv_album_settings.hpp"

namespace ams::capsrv {

    struct AlbumDateTime {
        u16 year;
        u8 month;
        u8 day;
        u8 hour;
        u8 minute;
        u8 second;
        u8 id;

        const char *AsString() const;
    };
    static_assert(sizeof(AlbumDateTime) == sizeof(CapsAlbumFileDateTime));

    struct AlbumFileId {
        u64 application_id;
        AlbumDateTime date_time;
        StorageId storage_id;
        ContentType type;
        u8 pad[0x6];

        Result Verify(AlbumSettings *settings) const;

        static Result FromString(AlbumFileId *file_id, StorageId storage, const char *str);
        ALWAYS_INLINE bool operator==(const AlbumFileId &file_id) {
            return memcmp(this, &file_id, sizeof(file_id)) == 0;
        }
        ALWAYS_INLINE bool operator!=(const AlbumFileId &file_id) {
            return !this->operator==(file_id);
        }
    };
    static_assert(sizeof(AlbumFileId) == sizeof(CapsAlbumFileId));

    struct AlbumEntry {
        s64 size;
        AlbumFileId fileId;

        ALWAYS_INLINE bool operator==(const AlbumEntry &f) {
            return memcmp(this, &f, sizeof(AlbumEntry)) == 0;
        }
        ALWAYS_INLINE bool operator!=(const AlbumEntry &f) {
            return !this->operator==(f);
        }
    };
    static_assert(sizeof(AlbumEntry) == sizeof(CapsAlbumEntry));

    union ApplicationAlbumEntry {
        u8 data[0x20];

        struct {
            u8 hash[0x20];
        } v0;

        struct {
            s64 size;
            u64 hash;
            AlbumDateTime date_time;
            StorageId storage_id;
            ContentType type;
            u8 pad_x1a[0x5];
            u8 unk_x1f;
        } v1;
    };
    static_assert(sizeof(ApplicationAlbumEntry) == sizeof(CapsApplicationAlbumEntry));

    struct ApplicationAlbumFileEntry {
        ApplicationAlbumEntry entry;
        AlbumDateTime datetime;
        u64 unk_x28;
    };
    static_assert(sizeof(ApplicationAlbumFileEntry) == sizeof(CapsApplicationAlbumFileEntry));

    struct Dimensions {
        u64 width;
        u64 height;
    };

    struct AlbumManagerCache {
        CapsAlbumCache cache[static_cast<u8>(StorageId::Count)][static_cast<u8>(ContentType::Count)]{};

        size_t GetCount(StorageId storage_id, ContentType type);
        void Set(StorageId storage_id, ContentType type, CapsAlbumCache cache);
        CapsAlbumCache Get(StorageId storage_id, ContentType type);
        void Increment(StorageId storage_id, ContentType type);
        void Decrement(StorageId storage_id, ContentType type);
    };

    struct AlbumReserve {
        struct _Reserve {
            bool used;
            AlbumFileId file_id;
        } reserve[10];

        AlbumReserve();
        ~AlbumReserve();
        Result Reserve(const AlbumFileId &file_id);
        Result Unreserve(const AlbumFileId &file_id);
        bool IsReserved(const AlbumFileId &file_id);
    };

    struct AlbumPath {
        char path_buffer[0x70];
        u32 unk;
        
        AlbumPath() : path_buffer(), unk() { /* ... */ }

        Result MakeAlbumPath(const AlbumFileId &file_id, u32 unk, AlbumSettings *settings);
    };

    struct AlbumUsage16 : sf::LargeData, sf::PrefersMapAliasTransferMode, CapsAlbumUsage16 {};
    struct LoadAlbumScreenShotImageOutput : sf::LargeData, sf::PrefersMapAliasTransferMode, CapsLoadAlbumScreenShotImageOutput {};
    struct LoadAlbumScreenShotImageOutputForApplication : sf::LargeData, sf::PrefersMapAliasTransferMode, CapsLoadAlbumScreenShotImageOutputForApplication {};

}
