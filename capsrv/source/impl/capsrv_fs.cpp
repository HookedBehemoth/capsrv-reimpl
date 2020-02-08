#include "capsrv_fs.hpp"

#include <machine/endian.h>

#include <filesystem>
#include <map>
#include <string>

#include "../capsrv_config.hpp"
#include "../capsrv_util.hpp"

namespace ams::capsrv::impl {

struct Storage {
	CapsAlbumCache usage[4];
};

constexpr const char *mountNames[] = {
	[StorageId::Nand] = "NA",
	[StorageId::Sd] = "SD"};

constexpr const char *mountPoints[] = {
	[StorageId::Nand] = "NA:/",
	[StorageId::Sd] = "SD:/"};

constexpr const char *fileExtensions[] = {
	[0] = ".jpg",
	[1] = ".mp4"};

namespace {

bool g_mountStatus[2] = {false, false};
Storage g_storage[2] = {0};
os::Mutex g_mutex;

StorageId GetPrimaryStorage() {
	SetSysPrimaryAlbumStorage storage;
	Result rc = setsysGetPrimaryAlbumStorage(&storage);

	if (R_FAILED(rc))
		std::abort();

	return StorageId(storage);
}

Result MountImageDirectory(StorageId storage) {
	FsFileSystem imgDirFs;
	R_TRY(fsOpenImageDirectoryFileSystem(&imgDirFs, FsImageDirectoryId(storage)));

	if (fsdevMountDevice(mountNames[storage], imgDirFs) == -1) {
    	fsFsClose(&imgDirFs);
		return capsrv::ResultFailedToMountImageDirectory();
		//std::abort();
	}

	return ResultSuccess();
}

Result MountCustomImageDirectory(const char *path) {
	if (nullptr)
		return fs::ResultNullptrArgument();

	// TODO: Mount Host PC when TMA is a thing.
	return fs::ResultHostFileSystemCorrupted();
}

Result UnmountImageDirectory(StorageId storage) {
	// TODO: Unmount Host PC when TMA is a thing.
	return fsdevUnmountDevice(mountNames[storage]);
}

const char *GetMountName(StorageId storage) {
	return mountNames[storage];
}

void cb_list(const Entry &entry, u8 *list, u8 **next) {
	memcpy(list, &entry, 0x20);
	*next = list + 0x20;
}

void cb_count(const Entry &entry, u8 *user, u8 **next) {
	(*(u64 *)user)++;
}

Result ProcessImageDirectory(StorageId storage, std::function<void(const Entry&, u8*, u8**)> callback, u8 *user) {
	/*for (auto &e : std::filesystem::recursive_directory_iterator(mountPoints[storage])) {
		if (e.is_regular_file()) {
			FileId fileId = {0};
			Result rc = FileId::FromString(&fileId, storage, e.path().filename().c_str());
			Entry entry = {
				.size = e.file_size(),
				.fileId = fileId,
			};
			if (rc.IsSuccess())
				callback(entry, user, &user);
		}
	}*/
	return ResultSuccess();
}

} // namespace

Result GetAlbumFileCount(u64 *outCount, StorageId storage, u8 flags) {
	R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
	u64 count = 0;
	Result res = 0;
	{
		std::scoped_lock lk(g_mutex);
		ProcessImageDirectory(storage, cb_count, (u8*)&count);
	}
	if (res.IsSuccess() && outCount)
		*outCount = count;
	return res;
}

Result GetAlbumFileList(void *ptr, u64 size, u64 *outCount, StorageId storage, u8 flags) {
	R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
	u64 count = 0;
	Result res = 0;
	{
		std::scoped_lock lk(g_mutex);
		ProcessImageDirectory(storage, cb_list, (u8*)ptr);
	}
	if (res.IsSuccess() && outCount)
		*outCount = count;
	return res;
}

Result DeleteAlbumFile(const FileId &fileId) {
	R_UNLESS(config::StorageValid(fileId.storage), capsrv::ResultInvalidStorageId());
	std::string path = fileId.GetFilePath();
	{
		std::scoped_lock lk{g_mutex};
	}
	return ResultSuccess();
}

Result GetAutoSavingStorage(StorageId *out) {
	StorageId storage = GetPrimaryStorage();
	R_UNLESS(MountAlbum(storage).IsSuccess(), capsrv::ResultFailedToMountImageDirectory());

	*out = storage;
	return ResultSuccess();
}

Result IsAlbumMounted(bool *out, StorageId storage) {
	R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());

	*out = g_mountStatus[storage];
	return ResultSuccess();
}

Result MountAlbum(StorageId storage) {
	std::scoped_lock lk{g_mutex};

	R_UNLESS(!g_mountStatus[storage], ResultSuccess());
	R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());

	const char *customDirectory = config::GetCustomDirectoryPath();
	if (storage == StorageId::Sd && customDirectory) {
		return MountCustomImageDirectory(customDirectory);
	}
	return MountImageDirectory(storage);

	g_mountStatus[storage] = true;
	return ResultSuccess();
}

Result UnmountAlbum(StorageId storage) {
	std::scoped_lock lk{g_mutex};

	R_UNLESS(g_mountStatus[storage], ResultSuccess());
	R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());

	return UnmountImageDirectory(storage);

	g_mountStatus[storage] = false;
	return ResultSuccess();
}

Result GetAlbumCache(CapsAlbumCache *out, StorageId storage, ContentType type) {
	R_UNLESS(config::StorageValid(storage), capsrv::ResultInvalidStorageId());
	R_UNLESS(config::SupportsType(type), capsrv::ResultInvalidContentType());

	*out = g_storage[storage].usage[type];

	return ResultSuccess();
}

} // namespace ams::capsrv::impl