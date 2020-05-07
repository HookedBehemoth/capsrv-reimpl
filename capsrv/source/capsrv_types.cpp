#include "capsrv_types.hpp"

#include "capsrv_crypto.hpp"
#include "capsrv_album_settings.hpp"

namespace ams::capsrv {

    namespace {

        Result DecryptFileIdentifier(u64 *applicationId, bool *isExtra, const char *str, const char **next) {
            union {
                u64 a[2];
                u8 b[0x10];
            } in;
            char buffer[3];
            char *end;
            buffer[2] = '\0';
            for (int i = 0; i < 0x10; i++) {
                /* Copy to temporary buffer. */
                buffer[0] = *(str++);
                buffer[1] = *(str++);
                /* Read buffer. */
                u8 tmp = strtoul(buffer, &end, 0x10);
                /* Check if number is valid. */
                if (end != &buffer[2] || tmp >= 0x100)
                    break;
                in.b[i] = tmp;
            }

            u64 out[2];
            crypto::path::Decrypt(out, in.a);

            *next = str;

            R_UNLESS(out[1] <= 1, capsrv::ResultAlbumInvalidApplicationId());
            *isExtra = out[1] == 1;
            *applicationId = out[0];
            return ResultSuccess();
        }

        Result GetFileType(ContentType *type, bool isExtra, const char *str) {
            for (u8 i = 0; i < 2; i++) {
                if (std::strcmp(GetFileExtension(ContentType(i)), str) != 0)
                    continue;
                *type = static_cast<ContentType>(i + (isExtra ? 2 : 0));
                return ResultSuccess();
            }
            return capsrv::ResultAlbumInvalidFileContents();
        }

        void GetFolderPath(u32 *out_length, char *buffer, u64 max_length, const AlbumFileId &file_id) {
            const char *mount_point = MountPoints[static_cast<u8>(file_id.storage_id)];

            u32 written = 0;

            size_t tmp;
            char *ptr = buffer;
            tmp = std::snprintf(ptr, max_length, "%s", mount_point);
            ptr += tmp;
            max_length -= tmp;
            written += tmp;

            if (IsExtra(file_id.type)) {
                tmp = std::snprintf(ptr, max_length, "%s", "Extra");
                ptr += tmp;
                max_length -= tmp;
                written += tmp;

                crypto::path::Cypher cypher = crypto::path::EncryptExtraPath(file_id.application_id);
                char cypher_string[0x20];
                for (int i = 0; i < 0x10; i++) {
                    std::snprintf(cypher_string + (2*i), sizeof(cypher_string) - (2*i), "%02X", cypher.c[i]);
                }
                tmp = std::snprintf(ptr, max_length, "%s", cypher_string);
                ptr += tmp;
                max_length -= tmp;
                written += tmp;
            }

            tmp = std::snprintf(ptr, max_length, "/%04d/%02d/%02d/",
                                file_id.date_time.year,
                                file_id.date_time.month,
                                file_id.date_time.day);

            *out_length = (written + tmp);
        }

        void GetFileName(u32 *out_length, char *buffer, size_t max, AlbumDateTime date_time, u64 application_id, ContentType type, AlbumSettings *settings) {
            crypto::path::Cypher cypher = crypto::path::EncryptFileExtension(application_id, IsExtra(type));
            char cypher_string[0x20];
            for (int i = 0; i < 0x10; i++) {
                std::snprintf(cypher_string + (2*i), sizeof(cypher_string) - (2*i), "%02X", cypher.c[i]);
            }
            const char *format = IsExtra(type) ? "%04d%02d%02d%02d%02d%02d%02d-%sX%s" : "%04d%02d%02d%02d%02d%02d%02d-%s%s";
            *out_length = std::snprintf(buffer, max, format,
                date_time.year,
                date_time.month,
                date_time.day,
                date_time.hour,
                date_time.minute,
                date_time.second,
                date_time.id,
                cypher_string,
                GetFileExtension(type)
            );
        }

        void GetAlbumPath(u32 *out_length, char *buffer, size_t max, const AlbumFileId &file_id, AlbumSettings *settings) {
            u32 written_path, written_file;

            /* Set folder path. */
            GetFolderPath(&written_path, buffer, max, file_id);

            /* Append file name. */
            GetFileName(&written_file, buffer + written_path, max - written_path, file_id.date_time, file_id.application_id, file_id.type, settings);
            *out_length = written_path + written_file;
        }

    }

    Result DateTimeFromString(AlbumDateTime *date, const char *str, const char **next) {
        AlbumDateTime tmp;
        tmp.year = (str[3] - 0x30) + (str[2] - 0x30) * 10 + (str[1] - 0x30) * 100 + (str[0] - 0x30) * 1000;
        R_UNLESS(tmp.year < 10000, capsrv::ResultAlbumInvalidFileId());

        tmp.month = (str[5] - 0x30) + (str[4] - 0x30) * 10;
        R_UNLESS(tmp.month - 1 < 12, capsrv::ResultAlbumInvalidFileId());

        tmp.day = (str[7] - 0x30) + (str[6] - 0x30) * 10;
        R_UNLESS(tmp.day - 1 < 31, capsrv::ResultAlbumInvalidFileId());

        tmp.hour = (str[9] - 0x30) + (str[8] - 0x30) * 10;
        R_UNLESS(tmp.hour < 24, capsrv::ResultAlbumInvalidFileId());

        tmp.minute = (str[11] - 0x30) + (str[10] - 0x30) * 10;
        R_UNLESS(tmp.minute < 60, capsrv::ResultAlbumInvalidFileId());

        tmp.second = (str[13] - 0x30) + (str[12] - 0x30) * 10;
        R_UNLESS(tmp.second < 60, capsrv::ResultAlbumInvalidFileId());

        tmp.id = (str[15] - 0x30) + (str[14] - 0x30) * 10;
        R_UNLESS(tmp.id < 100, capsrv::ResultAlbumInvalidFileId());

        *date = tmp;
        *next = str + 0x10;
        return ResultSuccess();
    }

    Result AlbumFileId::Verify(AlbumSettings *settings) const {
        R_UNLESS(this->application_id != 0,      capsrv::ResultAlbumInvalidFileId());
        R_UNLESS(this->date_time.year < 10000,   capsrv::ResultAlbumInvalidFileId());
        R_UNLESS(this->date_time.month - 1 < 12, capsrv::ResultAlbumInvalidFileId());
        R_UNLESS(this->date_time.day - 1 < 31,   capsrv::ResultAlbumInvalidFileId());
        R_UNLESS(this->date_time.hour < 24,      capsrv::ResultAlbumInvalidFileId());
        R_UNLESS(this->date_time.minute < 60,    capsrv::ResultAlbumInvalidFileId());
        R_UNLESS(this->date_time.second < 60,    capsrv::ResultAlbumInvalidFileId());
        R_UNLESS(this->date_time.id < 100,       capsrv::ResultAlbumInvalidFileId());
        R_TRY(VerifyStorageId(this->storage_id));
        R_UNLESS(settings->IsTypeSupported(this->type), capsrv::ResultAlbumInvalidFileContents());
        return ResultSuccess();
    }

    Result AlbumFileId::FromString(AlbumFileId *fileId, StorageId storage, const char *str) {
        size_t length = std::strlen(str);
        R_UNLESS(length == 0x35 || length == 0x36, 0x10);
        R_TRY(DateTimeFromString(&fileId->date_time, str, &str));
        R_UNLESS(*str == '-', capsrv::ResultAlbumInvalidFileId());
        str++;
        bool isExtra;
        R_TRY(DecryptFileIdentifier(&fileId->application_id, &isExtra, str, &str));
        if (isExtra) {
            R_UNLESS(*str == 'X', capsrv::ResultAlbumInvalidFileId());
            str++;
        }
        R_UNLESS(*str == '.', capsrv::ResultAlbumInvalidFileId());
        R_TRY(GetFileType(&fileId->type, isExtra, str));
        fileId->storage_id = storage;
        return ResultSuccess();
    }

    size_t AlbumManagerCache::GetCount(StorageId storage_id, ContentType type) {
        if (StorageValid(storage_id) && ContentTypeValid(type)) {
            return this->cache[static_cast<u8>(storage_id)][static_cast<u8>(type)].count;
        } else {
            return 0;
        }
    }

    void AlbumManagerCache::Set(StorageId storage_id, ContentType type, CapsAlbumCache cache) {
        if (StorageValid(storage_id) && ContentTypeValid(type))
            this->cache[static_cast<u8>(storage_id)][static_cast<u8>(type)] = cache;
    }

    CapsAlbumCache AlbumManagerCache::Get(StorageId storage_id, ContentType type) {
        return this->cache[static_cast<u8>(storage_id)][static_cast<u8>(type)];
    }

    void AlbumManagerCache::Increment(StorageId storage_id, ContentType type) {
        if (StorageValid(storage_id) && ContentTypeValid(type))
            this->cache[static_cast<u8>(storage_id)][static_cast<u8>(type)].count++;
    }

    void AlbumManagerCache::Decrement(StorageId storage_id, ContentType type) {
        if (StorageValid(storage_id) && ContentTypeValid(type))
            this->cache[static_cast<u8>(storage_id)][static_cast<u8>(type)].count--;
    }

    AlbumReserve::AlbumReserve() {
        for (auto res : this->reserve) {
            res.used = false;
        }
    }

    AlbumReserve::~AlbumReserve() {
        for (auto res : this->reserve) {
            res.used = false;
        }
    }

    Result AlbumReserve::Reserve(const AlbumFileId &file_id) {
        for (auto res : this->reserve) {
            if (res.used)
                continue;

            res.file_id = file_id;
            res.used = true;
            return ResultSuccess();
        }
        return capsrv::ResultInternalError();
    }

    Result AlbumReserve::Unreserve(const AlbumFileId &file_id) {
        for (auto res : this->reserve) {
            if (!res.used)
                continue;

            if (res.file_id == file_id) {
                res.used = false;
                return ResultSuccess();
            }
        }
        return capsrv::ResultInternalError();
    }

    bool AlbumReserve::IsReserved(const AlbumFileId &file_id) {
        for (auto res : this->reserve) {
            if (!res.used)
                continue;

            if (res.file_id == file_id) {
                return true;
            }
        }
        return false;
    }

    Result AlbumPath::MakeAlbumPath(const AlbumFileId &file_id, u32 unk, AlbumSettings *settings) {
        R_TRY(file_id.Verify(settings));

        u32 length;
        GetAlbumPath(&length, this->path_buffer, 0x6d, file_id, settings);
        this->unk = unk;
        return ResultSuccess();
    }

}