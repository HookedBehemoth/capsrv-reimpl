#include "capsrv_types.hpp"

#include <machine/endian.h>

#include "capsrv_config.hpp"
#include "capsrv_crypto.hpp"
#include "capsrv_util.hpp"

namespace ams::capsrv {

    namespace {

        constexpr const char *fileExtensions[] = {
            [0] = ".jpg",
            [1] = ".mp4",
        };

        Result DecryptFileIdentifier(u64 *applicationId, bool *isExtra, const char *str, const char **next) {
            u64 in[2] = {0};
            sscanf(str, "%16lX%16lX", &in[0], &in[1]);
            *next = str + 0x20;
            in[0] = __bswap64(in[0]);
            in[1] = __bswap64(in[1]);
            u64 out[2];
            crypto::aes128::Decrypt(out, in);

            R_UNLESS(out[1] <= 1, capsrv::ResultInvalidUnknown());
            *isExtra = out[1] == 1;
            *applicationId = out[0];
            return ResultSuccess();
        }

        Result GetFileType(ContentType *type, bool isExtra, const char *str) {
            for (u8 i = 0; i < 2; i++) {
                if (strcmp(fileExtensions[i], str) != 0)
                    continue;
                *type = static_cast<ContentType>(i + (isExtra ? 2 : 0));
                return ResultSuccess();
            }
            return capsrv::ResultInvalidContentType();
        }

    }

    std::string DateTime::AsString() const {
        char out[36];
        int size = snprintf(out, 36, "[%04d:%02d:%02d %02d:%02d:%02d %02d]",
                            this->year,
                            this->month,
                            this->day,
                            this->hour,
                            this->minute,
                            this->second,
                            this->id);
        return std::string(out, size);
    }

    Result DateTime::FromString(DateTime *date, const char *str, const char **next) {
        DateTime tmp;
        sscanf(str, "%4hd%2hhd%2hhd%2hhd%2hhd%2hhd%2hhd",
               &tmp.year,
               &tmp.month,
               &tmp.day,
               &tmp.hour,
               &tmp.minute,
               &tmp.second,
               &tmp.id);

        R_UNLESS(tmp.year < 10000, capsrv::ResultInvalidFileId());
        R_UNLESS(tmp.month - 1 < 12, capsrv::ResultInvalidFileId());
        R_UNLESS(tmp.day - 1 < 31, capsrv::ResultInvalidFileId());
        R_UNLESS(tmp.hour < 24, capsrv::ResultInvalidFileId());
        R_UNLESS(tmp.minute < 60, capsrv::ResultInvalidFileId());
        R_UNLESS(tmp.second < 60, capsrv::ResultInvalidFileId());
        R_UNLESS(tmp.id < 100, capsrv::ResultInvalidFileId());

        *date = tmp;
        if (next)
            *next = str + 0x10;
        return ResultSuccess();
    }

    std::string FileId::AsString() const {
        char out[60];
        int size = snprintf(out, 60, "%016lx, %s, %hhd, %hhd",
                            this->applicationId,
                            this->datetime.AsString().c_str(),
                            this->storage, this->type);
        return std::string(out, size);
    }

    std::string FileId::GetFolderPath() const {
        bool isExtra = this->type > 1;
        if (isExtra) {
            const u64 in[2] = {this->applicationId, 0};
            u64 aes[2] = {0};
            crypto::aes128::Encrypt(aes, in);
            aes[0] = __bswap64(aes[0]);
            aes[1] = __bswap64(aes[1]);
            // TODO: std::fmt
            char buf[0x38]{};
            int size = std::snprintf(buf, 0x38, "Extra/%02lX%02lX/%04hd/%02hhd/%02hhd/",
                                     aes[0],
                                     aes[1],
                                     this->datetime.year,
                                     this->datetime.month,
                                     this->datetime.day);
            return std::string(buf, size);
        } else {
            char buf[0x10]{};
            int size = std::snprintf(buf, 0x10, "%04hd/%02hhd/%02hhd/",
                                     this->datetime.year,
                                     this->datetime.month,
                                     this->datetime.day);
            return std::string(buf, size);
        }
    }

    std::string FileId::GetFileName() const {
        bool isExtra = (u8)this->type > 1;
        const u64 in[2] = {this->applicationId, isExtra};
        u64 aes[2] = {0};
        crypto::aes128::Encrypt(aes, in);
        aes[0] = __bswap64(aes[0]);
        aes[1] = __bswap64(aes[1]);

        char buf[0x36]{};
        const char *fmt = isExtra ? "%04d%02d%02d%02d%02d%02d%02d-%lX%lXX%s" : "%04d%02d%02d%02d%02d%02d%02d-%lX%lX%s";
        int size = std::snprintf(buf, 0x36, fmt,
                                 this->datetime.year,
                                 this->datetime.month,
                                 this->datetime.day,
                                 this->datetime.hour,
                                 this->datetime.minute,
                                 this->datetime.second,
                                 this->datetime.id,
                                 aes[0], aes[1],
                                 fileExtensions[this->type % 2]);
        return std::string(buf, size);
    }

    std::string FileId::GetFilePath() const {
        return this->GetFolderPath() + this->GetFileName();
    }

    Result FileId::Verify() const {
        R_UNLESS(this->applicationId != 0, capsrv::ResultInvalidFileId());
        R_UNLESS(this->datetime.year < 10000, capsrv::ResultInvalidFileId());
        R_UNLESS(this->datetime.month - 1 < 12, capsrv::ResultInvalidFileId());
        R_UNLESS(this->datetime.day - 1 < 31, capsrv::ResultInvalidFileId());
        R_UNLESS(this->datetime.hour < 24, capsrv::ResultInvalidFileId());
        R_UNLESS(this->datetime.minute < 60, capsrv::ResultInvalidFileId());
        R_UNLESS(this->datetime.second < 60, capsrv::ResultInvalidFileId());
        R_UNLESS(this->datetime.id < 100, capsrv::ResultInvalidFileId());
        R_UNLESS(config::StorageValid(this->storage), capsrv::ResultInvalidStorageId());
        R_UNLESS(config::SupportsType(this->type), capsrv::ResultInvalidContentType());
        return ResultSuccess();
    }

    Result FileId::FromString(FileId *fileId, StorageId storage, const char *str) {
        size_t length = strlen(str);
        R_UNLESS(length == 0x35 || length == 0x36, 0x10);
        R_TRY(DateTime::FromString(&fileId->datetime, str, &str));
        R_UNLESS(*str == '-', capsrv::ResultInvalidFileId());
        str++;
        bool isExtra;
        R_TRY(DecryptFileIdentifier(&fileId->applicationId, &isExtra, str, &str));
        if (isExtra) {
            R_UNLESS(*str == 'X', capsrv::ResultInvalidFileId());
            str++;
        }
        R_UNLESS(*str == '.', capsrv::ResultInvalidFileId());
        R_TRY(GetFileType(&fileId->type, isExtra, str));
        fileId->storage = storage;
        return ResultSuccess();
    }

    std::string Entry::AsString() const {
        char out[120];
        int size = snprintf(out, 120, "[%ld, %s]", this->size, this->fileId.AsString().c_str());
        return std::string(out, size);
    }

    Result ContentStorage::CanSave(StorageId storage, ContentType type) const {
        R_TRY(config::StorageValid(storage));
        R_TRY(config::SupportsType(type));
        u64 max = config::GetMax(storage, type);
        u64 currentCount = this->cache[storage][type].count;
        R_UNLESS(currentCount < max, capsrv::ResultTooManyFiles());
        return ResultSuccess();
    }

    void ContentStorage::Set(StorageId storage, ContentType type, CapsAlbumCache cache) {
        if (storage < 2 && type < 4)
            this->cache[storage][type] = cache;
    }

    void ContentStorage::Increment(StorageId storage, ContentType type) {
        if (storage < 2 && type < 4)
            this->cache[storage][type].count++;
    }

    void ContentStorage::Decrement(StorageId storage, ContentType type) {
        if (storage < 2 && type < 4)
            this->cache[storage][type].count--;
    }

}