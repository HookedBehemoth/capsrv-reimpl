#include "capsrv_types.hpp"

#include "capsrv_crypto.hpp"
#include "capsrv_util.hpp"

namespace ams::capsrv {

namespace {

constexpr const char *mountNames[] = {
	[StorageId::Nand] = "NA",
	[StorageId::Sd] = "SD"};

constexpr const char *mountPoints[] = {
	[StorageId::Nand] = "NA:/",
	[StorageId::Sd] = "NA:/"};

constexpr const char *fileExtensions[] = {
	[0] = ".jpg",
	[1] = ".mp4"};

Result DecryptFileIdentifier(u64 *applicationId, bool *isExtra, const char *str, const char **next) {
	u64 in[2] = {0};
	sscanf(str, "%16lX%16lX", &in[0], &in[1]);
	*next = str + 0x20;
	in[0] = __bswap64(in[0]);
	in[1] = __bswap64(in[1]);
	u64 out[2];
	crypto::aes128::Decrypt(out, in);

	R_UNLESS(out[1] <= 1, 0x16ce);
	*isExtra = out[1] == 1;
	*applicationId = out[0];
	return ResultSuccess();
}

Result GetFileType(ContentType *type, bool isExtra, const char *str) {
	for (u8 i = 0; i < 2; i++) {
		if (strcmp(fileExtensions[i], str) != 0)
			continue;
		*type = static_cast<ContentType>(i + isExtra ? 2 : 0);
		return ResultSuccess();
	}
	return capsrv::ResultInvalidContentType();
}

} // namespace

std::string DateTime::AsString() const {
	char out[36];
	snprintf(out, 36, "[%04d:%02d:%02d %02d:%02d:%02d %02d]",
			 this->year,
			 this->month,
			 this->day,
			 this->hour,
			 this->minute,
			 this->second,
			 this->id);
	return std::string(out);
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

	R_UNLESS(tmp.year < 2700, capsrv::ResultInvalidFileId());
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
	snprintf(out, 60, "%016lx, %s, %hhd, %hhd",
			 this->applicationId,
			 this->datetime.AsString().c_str(),
			 this->storage, this->type);
	return std::string(out);
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
		char buf[0x38];
		int size = std::snprintf(buf, 0x38, "%sExtra/%02lX%02lX/%04hd/%02hhd/%02hhd/",
								 mountPoints[this->storage],
								 aes[0],
								 aes[1],
								 this->datetime.year,
								 this->datetime.month,
								 this->datetime.day);
		return std::string(buf, size);
	} else {
		// TODO: std::fmt
		char buf[0x10];
		int size = std::snprintf(buf, 0x10, "%s%04hd/%02hhd/%02hhd/",
								 mountPoints[this->storage],
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
	// TODO: std::fmt
	char buf[0x36];
	const char *fmt = isExtra ? "%04d%02d%02d%02d%02d%02d%02d-%ld%ld%s" : "%04d%02d%02d%02d%02d%02d%02d-%lX%lXX%s";
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

Result FileId::FromString(FileId *fileId, StorageId storage, const char *str) {
	R_UNLESS(strlen(str) == 0x35, 0x10);
	R_TRY(DateTime::FromString(&fileId->datetime, str, &str));
	R_UNLESS(*str == '-', 0x18ce);
	str++;
	bool isExtra;
	R_TRY(DecryptFileIdentifier(&fileId->applicationId, &isExtra, str, &str));
	R_UNLESS(*str == '.', 0x18ce);
	R_TRY(GetFileType(&fileId->type, isExtra, str));
	fileId->storage = storage;
	return ResultSuccess();
}

std::string ApplicationFileId::AsString() const {
	char out[120];
	snprintf(out, 120, "%016lx, %016lx, %s, %016lx", this->unk_x0, this->unk_x8, this->datetime.AsString().c_str(), this->unk_x18);
	return std::string(out);
}

std::string ApplicationEntry::AsString() const {
	char out[200];
	snprintf(out, 200, "%s, %s, %016lx", this->fileId.AsString().c_str(), this->datetime.AsString().c_str(), this->unk_x28);
	return std::string(out);
}

} // namespace ams::capsrv