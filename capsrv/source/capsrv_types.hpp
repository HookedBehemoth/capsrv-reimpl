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

struct FileId {
	u64 applicationId;
	DateTime datetime;
	StorageId storage;
	ContentType type;

	std::string AsString() const;
    std::string GetFolderPath() const;
    std::string GetFileName() const;
    std::string GetFilePath() const;

	static Result FromString(FileId *fileId, StorageId storage, const char *str);
};

struct Entry {
	u64 size;
	FileId fileId;
};

struct ApplicationFileId {
	u64 unk_x0;
	u64 unk_x8;
	DateTime datetime;
	u64 unk_x18;

	std::string AsString() const;
};

struct ApplicationEntry {
	ApplicationFileId fileId;
	DateTime datetime;
	u64 unk_x28;

	std::string AsString() const;
};

struct Dimensions {
	u64 width;
	u64 height;
};

struct AlbumUsage16 : sf::LargeData, sf::PrefersMapAliasTransferMode, CapsAlbumUsage16 {};
struct LoadAlbumScreenShotImageOutput : sf::LargeData, sf::PrefersMapAliasTransferMode, CapsLoadAlbumScreenShotImageOutput {};
struct LoadAlbumScreenShotImageOutputForApplication : sf::LargeData, sf::PrefersMapAliasTransferMode, CapsLoadAlbumScreenShotImageOutputForApplication {};

} // namespace ams::capsrv