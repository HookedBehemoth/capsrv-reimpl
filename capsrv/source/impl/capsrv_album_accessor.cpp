#include "capsrv_album_accessor.hpp"
#include <stratosphere.hpp>

namespace ams::capsrv::impl {

    namespace {
        os::Mutex g_mutex;
    }

    Result InitializeAlbumAccessor() {
        
    }

    void FinalizeAlbumAccessor() {

    }

    Result GetAlbumFileCount(u64* outCount, const StorageId storageId) {

    }

    Result GetAlbumFileList(void* ptr, u64 size, u64* outCount, const StorageId storageId) {

    }

    Result LoadAlbumFile(void* ptr, u64 size, u64* outSize, const CapsAlbumFileId &fileId) {

    }

}