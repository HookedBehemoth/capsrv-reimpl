#include "capsrv_album_accessor_service.hpp"

#include "../impl/capsrv_controller.hpp"
#include "../impl/capsrv_fs.hpp"
#include "../impl/capsrv_image.hpp"
#include "../impl/capsrv_overlay.hpp"
#include "../logger.hpp"

namespace ams::capsrv {

    Result AlbumAccessorService::GetAlbumFileCount(sf::Out<u64> out, StorageId storage) {
        WriteLogFile("Accessor", "GetAlbumFileCount: storage(%hhd)", storage);
        return impl::GetAlbumFileCount(out.GetPointer(), storage, CapsAlbumFileContentsFlag_ScreenShot | CapsAlbumFileContentsFlag_Movie);
    }

    Result AlbumAccessorService::GetAlbumFileList(sf::OutBuffer buffer, sf::Out<u64> out, StorageId storage) {
        WriteLogFile("Accessor", "GetAlbumFileList: storage(%hhd), bufferSize(%ld)", storage, buffer.GetSize());
        return impl::GetAlbumFileList(buffer.GetPointer(), buffer.GetSize(), out.GetPointer(), storage, CapsAlbumFileContentsFlag_ScreenShot | CapsAlbumFileContentsFlag_Movie);
    }

    Result AlbumAccessorService::LoadAlbumFile(sf::OutBuffer image, sf::Out<u64> out, const FileId &fileId) {
        WriteLogFile("Accessor", "LoadAlbumFile: fileId(%s), bufferSize(%ld)", fileId.AsString().c_str(), image.GetSize());
        return impl::LoadAlbumFile(image.GetPointer(), image.GetSize(), out.GetPointer(), fileId);
    }

    Result AlbumAccessorService::DeleteAlbumFile(const FileId &fileId) {
        WriteLogFile("Accessor", "DeleteAlbumFile: fileId(%s)", fileId.AsString().c_str());
        return impl::DeleteAlbumFile(fileId);
    }

    Result AlbumAccessorService::StorageCopyAlbumFile(StorageId storage, const FileId &fileId) {
        WriteLogFile("Accessor", "StorageCopyAlbumFile: fileId(%s), storage(%hhd)", fileId.AsString().c_str(), storage);
        return impl::CopyAlbumFile(storage, fileId);
    }

    Result AlbumAccessorService::IsAlbumMounted(sf::Out<bool> out, StorageId storage) {
        WriteLogFile("Accessor", "IsAlbumMounted: storage(%hhd)", storage);
        return impl::IsAlbumMounted(out.GetPointer(), storage);
    }

    Result AlbumAccessorService::GetAlbumUsage(sf::Out<CapsAlbumUsage2> out, StorageId storage) {
        WriteLogFile("Accessor", "GetAlbumUsage: storage(%hhd)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumFileSize(sf::Out<u64> out, const FileId &fileId) {
        WriteLogFile("Accessor", "GetAlbumFileSize: fileId(%s)", fileId.AsString().c_str());
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumFileThumbnail(sf::OutBuffer image, sf::Out<u64> out, const FileId &fileId) {
        WriteLogFile("Accessor", "LoadAlbumFileThumbnail: imgSize(%ld), fileId(%s)", image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumFileThumbnail(image.GetPointer(), image.GetSize(), out.GetPointer(), fileId);
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImage(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId) {
        WriteLogFile("Accessor", "LoadAlbumScreenShotImage: workSize(%ld), imgSize(%ld), fileId(%s)", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotImage(width.GetPointer(), height.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), fileId);
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImage(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId) {
        WriteLogFile("Accessor", "LoadAlbumScreenShotThumbnailImage: workSize(%ld), imgSize(%ld), fileId(%s)", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotThumbnailImage(width.GetPointer(), height.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), fileId);
    }

    Result AlbumAccessorService::GetAlbumEntryFromApplicationAlbumEntry(sf::Out<Entry> out, const ApplicationEntry &appEntry, u64 appId) {
        WriteLogFile("Accessor", "GetAlbumEntryFromApplicationAlbumEntry: appEntry(#), appId(%016lx)", appId);
        return control::GetAlbumEntryFromApplicationAlbumEntry(out.GetPointer(), &appEntry, appId);
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImageEx(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile("Accessor", "LoadAlbumScreenShotImageEx: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotImageEx(width.GetPointer(), height.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), fileId, opts);
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImageEx(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile("Accessor", "LoadAlbumScreenShotThumbnailImageEx: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotThumbnailImageEx(width.GetPointer(), height.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), fileId, opts);
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImageEx0(sf::Out<CapsScreenShotAttribute> attrs, sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile("Accessor", "LoadAlbumScreenShotImageEx0: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotImageEx0(attrs.GetPointer(), width.GetPointer(), height.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), fileId, opts);
    }

    Result AlbumAccessorService::GetAlbumUsage3(sf::Out<CapsAlbumUsage3> usage, StorageId storage) {
        WriteLogFile("Accessor", "GetAlbumUsage3: storage(%d)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumMountResult(StorageId storage) {
        WriteLogFile("Accessor", "GetAlbumMountResult: storage(%d)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumUsage16(sf::Out<AlbumUsage16> usage, StorageId storage, u8 flags) {
        WriteLogFile("Accessor", "GetAlbumUsage16: storage(%hhd), flags(%hhd)", storage, flags);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetMinMaxAppletId(sf::OutNonSecureBuffer minMax, sf::Out<bool> success) {
        WriteLogFile("Accessor", "GetMinMaxAppletId");
        if (minMax.GetSize() != 0) {
            u64 *arr = (u64 *)minMax.GetPointer();
            arr[0] = 0x100000000001000;
            arr[1] = 0x100000000001fff;
            success.SetValue(true);
        }
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumFileCountEx0(sf::Out<u64> out, StorageId storage, u8 flags) {
        WriteLogFile("Accessor", "GetAlbumFileCountEx0: storage(%hhd), flags(%hhd)", storage, flags);
        return impl::GetAlbumFileCount(out.GetPointer(), storage, flags);
    }

    Result AlbumAccessorService::GetAlbumFileListEx0(sf::OutBuffer buffer, sf::Out<u64> out, u64 storage, u64 flags) {
        WriteLogFile("Accessor", "GetAlbumFileListEx0: bufferSize(%ld), storage(%ld), flags(%ld)", buffer.GetSize(), storage, flags);
        return impl::GetAlbumFileList(buffer.GetPointer(), buffer.GetSize(), out.GetPointer(), StorageId(storage), flags);
    }

    //Result AlbumAccessorService::SaveEditedScreenShot
    //Result GetLastOverlayThumbnail(void *ptr, u64 size, FileId *fileId, bool isMovie)
    Result AlbumAccessorService::GetLastOverlayScreenShotThumbnail(sf::OutBuffer buffer, sf::Out<FileId> out, sf::Out<u64> size) {
        WriteLogFile("Accessor", "GetLastOverlayScreenShotThumbnail: bufferSize(%ld)", buffer.GetSize());
        return ovl::GetLastOverlayThumbnail(buffer.GetPointer(), buffer.GetSize(), out.GetPointer(), size.GetPointer(), false);
    }

    Result AlbumAccessorService::GetLastOverlayMovieThumbnail(sf::OutBuffer buffer, sf::Out<FileId> out, sf::Out<u64> size) {
        WriteLogFile("Accessor", "GetLastOverlayScreenShotThumbnail: bufferSize(%ld)", buffer.GetSize());
        return ovl::GetLastOverlayThumbnail(buffer.GetPointer(), buffer.GetSize(), out.GetPointer(), size.GetPointer(), true);
    }

    Result AlbumAccessorService::GetAutoSavingStorage(sf::Out<StorageId> out) {
        WriteLogFile("Accessor", "GetAutoSavingStorage");
        return impl::GetAutoSavingStorage(out.GetPointer());
    }

    Result AlbumAccessorService::GetRequiredStorageSpaceSizeToCopyAll(sf::Out<u64> out, StorageId dst, StorageId src) {
        WriteLogFile("Accessor", "GetRequiredStorageSpaceSizeToCopyAll: dst(%hhd), src(%hhd)", dst, src);
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImageEx0(sf::Out<CapsScreenShotAttribute> attrs, sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile("Accessor", "LoadAlbumScreenShotImageEx0: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotThumbnailImageEx0(attrs.GetPointer(), width.GetPointer(), height.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), fileId, opts);
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImageEx1(sf::Out<LoadAlbumScreenShotImageOutput> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile("Accessor", "LoadAlbumScreenShotImageEx0: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotImageEx1(out.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), fileId, opts);
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImageEx1(sf::Out<LoadAlbumScreenShotImageOutput> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile("Accessor", "LoadAlbumScreenShotImageEx0: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotThumbnailImageEx1(out.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), fileId, opts);
    }

    Result AlbumAccessorService::ForceAlbumUnmounted(StorageId storage) {
        WriteLogFile("Accessor", "ForceAlbumUnmounted: storage(%hhd)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::ResetAlbumMountStatus(StorageId storage) {
        WriteLogFile("Accessor", "ResetAlbumMountStatus: storage(%hhd)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::RefreshAlbumCache(StorageId storage) {
        WriteLogFile("Accessor", "RefreshAlbumCache: storage(%hhd)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumCache(sf::Out<CapsAlbumCache> cache, StorageId storage) {
        WriteLogFile("Accessor", "GetAlbumCache: storage(%hhd)", storage);
        return impl::GetAlbumCache(cache.GetPointer(), storage, ContentType::Screenshot);
    }

    Result AlbumAccessorService::GetAlbumCacheEx(sf::Out<CapsAlbumCache> cache, StorageId storage, const ContentType type) {
        WriteLogFile("Accessor", "GetAlbumCacheEx: storage(%hhd), type(%hhd)", storage, type);
        return impl::GetAlbumCache(cache.GetPointer(), storage, type);
    }

    Result AlbumAccessorService::GetAlbumEntryFromApplicationAlbumEntryAruid(sf::Out<Entry> out, const sf::ClientAppletResourceUserId &aruid, const ApplicationEntry &appEntry) {
        WriteLogFile("Accessor", "GetAlbumEntryFromApplicationAlbumEntryAruid: ApplicationEntry(#), aruid(%ld)", aruid.GetValue());
        return control::GetAlbumEntryFromApplicationAlbumEntryAruid(out.GetPointer(), &appEntry, aruid.GetValue().value);
    }

    Result AlbumAccessorService::SetInternalErrorConversionEnabled(const bool enabled) {
        WriteLogFile("Accessor", "SetInternalErrorConversionEnabled: enabled(%s)", enabled ? "true" : "false");
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadMakerNoteInfoForDebug(sf::Out<u64> out, sf::OutBuffer unk0, sf::OutBuffer unk1, const FileId &fileId) {
        WriteLogFile("Accessor", "LoadMakerNoteInfoForDebug: bufferSize(%ld), bufferSize(%ld), fileId(%s)", unk0.GetSize(), unk1.GetSize(), fileId.AsString().c_str());
        return ResultSuccess();
    }

    Result AlbumAccessorService::OpenAccessorSession(sf::Out<std::shared_ptr<AccessorSession>> out, const sf::ClientAppletResourceUserId &aruid) {
        WriteLogFile("Accessor", "OpenAccessorSession: aruid(%ld)", aruid.GetValue());
        auto session = std::make_shared<AccessorSession>();
        out.SetValue(std::move(session));
        return ResultSuccess();
    }

}
