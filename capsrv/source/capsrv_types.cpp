#include "capsrv_types.hpp"

#include "capsrv_config.hpp"
#include "capsrv_crypto.hpp"
#include "capsrv_time.hpp"

#define DATE_FORMAT "%04d%02d%02d%02d%02d%02d%02d"
#define CYPHER_FORMAT "%016lX%016lX"
#define FILENAME_FORMAT DATE_FORMAT "-" CYPHER_FORMAT
#define DATE_PATH_FORMAT "/%04hd/%02hhd/%02hhd/"

namespace ams::capsrv {

    namespace {

        constexpr const char *const file_extensions[] = {
            ".jpg",
            ".mp4",
        };

        constexpr const char *const mount_names[] = {
            "NA",
            "SD",
        };

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
            crypto::aes128::Decrypt(out, in.a);

            *next = str;

            R_UNLESS(out[1] <= 1, capsrv::ResultInvalidUnknown());
            *isExtra = out[1] == 1;
            *applicationId = out[0];
            return ResultSuccess();
        }

        Result GetFileType(ContentType *type, bool isExtra, const char *str) {
            for (u8 i = 0; i < 2; i++) {
                if (std::strcmp(file_extensions[i], str) != 0)
                    continue;
                *type = static_cast<ContentType>(i + (isExtra ? 2 : 0));
                return ResultSuccess();
            }
            return capsrv::ResultInvalidContentType();
        }

    }

    const char *DateTime::AsString() const {
        static char date_buffer[32];
        std::snprintf(date_buffer, 32, "[%04d:%02d:%02d %02d:%02d:%02d %02d]",
                      this->year,
                      this->month,
                      this->day,
                      this->hour,
                      this->minute,
                      this->second,
                      this->id);
        return date_buffer;
    }

    Result DateTimeFromString(DateTime *date, const char *str, const char **next) {
        DateTime tmp;
        tmp.year = (str[3] - 0x30) + (str[2] - 0x30) * 10 + (str[1] - 0x30) * 100 + (str[0] - 0x30) * 1000;
        R_UNLESS(tmp.year < 10000, capsrv::ResultInvalidFileId());

        tmp.month = (str[5] - 0x30) + (str[4] - 0x30) * 10;
        R_UNLESS(tmp.month - 1 < 12, capsrv::ResultInvalidFileId());

        tmp.day = (str[7] - 0x30) + (str[6] - 0x30) * 10;
        R_UNLESS(tmp.day - 1 < 31, capsrv::ResultInvalidFileId());

        tmp.hour = (str[9] - 0x30) + (str[8] - 0x30) * 10;
        R_UNLESS(tmp.hour < 24, capsrv::ResultInvalidFileId());

        tmp.minute = (str[11] - 0x30) + (str[10] - 0x30) * 10;
        R_UNLESS(tmp.minute < 60, capsrv::ResultInvalidFileId());

        tmp.second = (str[13] - 0x30) + (str[12] - 0x30) * 10;
        R_UNLESS(tmp.second < 60, capsrv::ResultInvalidFileId());

        tmp.id = (str[15] - 0x30) + (str[14] - 0x30) * 10;
        R_UNLESS(tmp.id < 100, capsrv::ResultInvalidFileId());

        *date = tmp;
        *next = str + 0x10;
        return ResultSuccess();
    }

    const char *FileId::AsString() const {
        static char string_buffer[25];
        std::snprintf(string_buffer, 25, "[%016lx, %hhd, %hhd]", this->applicationId, this->storage, this->type);
        return string_buffer;
    }

    u64 FileId::GetFolderPath(char *buffer, u64 max_length) const {
        const char *mount_point = GetMountName(this->storage);
        if (this->IsExtra()) {
            const u64 in[2] = {this->applicationId, 0};
            u64 aes[2] = {0};
            crypto::aes128::Encrypt(aes, in);
            aes[0] = ams::util::ConvertToBigEndian(aes[0]);
            aes[1] = ams::util::ConvertToBigEndian(aes[1]);

            return std::snprintf(buffer, max_length, "%s:/Extra/" CYPHER_FORMAT DATE_PATH_FORMAT,
                                 mount_point,
                                 aes[0],
                                 aes[1],
                                 this->datetime.year,
                                 this->datetime.month,
                                 this->datetime.day);
        } else {
            return std::snprintf(buffer, max_length, "%s:" DATE_PATH_FORMAT,
                                 mount_point,
                                 this->datetime.year,
                                 this->datetime.month,
                                 this->datetime.day);
        }
    }

    u64 FileId::GetFileName(char *buffer, u64 max_length) const {
        bool isExtra = this->IsExtra();
        const u64 in[2] = {this->applicationId, isExtra};
        u64 aes[2] = {0};
        crypto::aes128::Encrypt(aes, in);
        aes[0] = ams::util::ConvertToBigEndian(aes[0]);
        aes[1] = ams::util::ConvertToBigEndian(aes[1]);

        const char *fmt = isExtra ? FILENAME_FORMAT "X%s" : FILENAME_FORMAT "%s";
        return std::snprintf(buffer, max_length, fmt,
                             this->datetime.year,
                             this->datetime.month,
                             this->datetime.day,
                             this->datetime.hour,
                             this->datetime.minute,
                             this->datetime.second,
                             this->datetime.id,
                             aes[0], aes[1],
                             GetFileExtension(this->type));
    }

    u64 FileId::GetFilePath(char *buffer, u64 max_length) const {
        /* Get Path to file. */
        u64 folder_path_length = this->GetFolderPath(buffer, max_length);
        /* Append filename. */
        u64 file_name_length = this->GetFileName(buffer + folder_path_length, max_length - folder_path_length);
        return folder_path_length + file_name_length;
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
        R_TRY(DateTimeFromString(&fileId->datetime, str, &str));
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

    const char *Entry::AsString() const {
        return this->fileId.AsString();
    }

    const char *GetFileExtension(ContentType type) {
        return file_extensions[type];
    }

    const char *GetMountName(StorageId storage) {
        return mount_names[storage];
    }

    Result ContentStorage::CanSave(StorageId storage, ContentType type) const {
        R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
        R_UNLESS(config::SupportsType(type), capsrv::ResultInvalidContentType());
        u64 max = config::GetMax(storage, type);
        u64 currentCount = this->cache[storage][type].count;
        R_UNLESS(currentCount < max, capsrv::ResultTooManyFiles());
        return ResultSuccess();
    }

    void ContentStorage::Set(StorageId storage, ContentType type, CapsAlbumCache cache) {
        if (config::StorageValid(storage) && config::ContentTypeValid(type))
            this->cache[storage][type] = cache;
    }

    void ContentStorage::Increment(StorageId storage, ContentType type) {
        if (config::StorageValid(storage) && config::ContentTypeValid(type))
            this->cache[storage][type].count++;
    }

    void ContentStorage::Decrement(StorageId storage, ContentType type) {
        if (config::StorageValid(storage) && config::ContentTypeValid(type))
            this->cache[storage][type].count--;
    }

}