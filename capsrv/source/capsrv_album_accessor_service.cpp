#include "capsrv_album_accessor_service.hpp"
#include "impl/capsrv_manager.hpp"
#include "impl/capsrv_image.hpp"

#include "logger.hpp"

namespace ams::capsrv {

    Result AlbumAccessorService::GetAlbumFileCount(sf::Out<u64> out, StorageId storage) {
        WriteLogFile(LogType_Info, "GetAlbumFileCount: storage(%hhd)", storage);
        return impl::GetAlbumFileCount(out.GetPointer(), storage);
    }

    Result AlbumAccessorService::GetAlbumFileList(sf::OutBuffer buffer, sf::Out<u64> out, StorageId storage) {
        WriteLogFile(LogType_Info, "GetAlbumFileList: storage(%hhd), bufferSize(%ld)", storage, buffer.GetSize());
        return impl::GetAlbumFileList(buffer.GetPointer(), buffer.GetSize(), out.GetPointer(), storage);
    }

    Result AlbumAccessorService::LoadAlbumFile(sf::OutBuffer image, sf::Out<u64> out, const FileId &fileId) {
        WriteLogFile(LogType_Info, "LoadAlbumFile: fileId(%s), bufferSize(%ld)", fileId.AsString().c_str(), image.GetSize());
        return impl::LoadAlbumFile(image.GetPointer(), image.GetSize(), out.GetPointer(), fileId);
    }

    Result AlbumAccessorService::DeleteAlbumFile(const FileId &fileId) {
        WriteLogFile(LogType_Info, "DeleteAlbumFile: fileId(%s)", fileId.AsString().c_str());
        return ResultSuccess();
    }

    Result AlbumAccessorService::StorageCopyAlbumFile(StorageId storage, const FileId &fileId) {
        WriteLogFile(LogType_Info, "StorageCopyAlbumFile: fileId(%s), storage(%hhd)", fileId.AsString().c_str(), storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::IsAlbumMounted(sf::Out<bool> out, StorageId storage) {
        WriteLogFile(LogType_Info, "IsAlbumMounted: storage(%hhd)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumUsage(sf::Out<CapsAlbumUsage2> out, StorageId storage) {
        WriteLogFile(LogType_Info, "GetAlbumUsage: storage(%hhd)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumFileSize(sf::Out<u64> out, const FileId &fileId) {
        WriteLogFile(LogType_Info, "GetAlbumFileSize: fileId(%s)", fileId.AsString().c_str());
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumFileThumbnail(sf::OutBuffer image, sf::Out<u64> out, const FileId &fileId) {
        WriteLogFile(LogType_Info, "LoadAlbumFileThumbnail: imgSize(%ld), fileId(%s)", image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumFileThumbnail(image.GetPointer(), image.GetSize(), out.GetPointer(), fileId);
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImage(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId) {
        WriteLogFile(LogType_Info, "LoadAlbumScreenShotImage: workSize(%ld), imgSize(%ld), fileId(%s)", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotImage(width.GetPointer(), height.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), fileId);
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImage(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId) {
        WriteLogFile(LogType_Info, "LoadAlbumScreenShotThumbnailImage: workSize(%ld), imgSize(%ld), fileId(%s)", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotThumbnailImage(width.GetPointer(), height.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), fileId);
    }

    Result AlbumAccessorService::GetAlbumEntryFromApplicationAlbumEntry(sf::Out<CapsAlbumEntry> out, const ApplicationEntry &appEntry, u64 appId) {
        WriteLogFile(LogType_Info, "GetAlbumEntryFromApplicationAlbumEntry: appEntry(%s), appId(%016lx)", appEntry.AsString().c_str(), appId);
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImageEx(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile(LogType_Info, "LoadAlbumScreenShotImageEx: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotImageEx(width.GetPointer(), height.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), fileId, opts);
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImageEx(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile(LogType_Info, "LoadAlbumScreenShotThumbnailImageEx: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotThumbnailImageEx(width.GetPointer(), height.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), fileId, opts);
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImageEx0(sf::Out<CapsScreenShotAttribute> attrs, sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile(LogType_Info, "LoadAlbumScreenShotImageEx0: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotImageEx0(attrs.GetPointer(), width.GetPointer(), height.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), fileId, opts);
    }

    Result AlbumAccessorService::GetAlbumUsage3(sf::Out<CapsAlbumUsage3> usage, StorageId storage) {
        WriteLogFile(LogType_Info, "GetAlbumUsage3: storage(%d)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumMountResult(StorageId storage) {
        WriteLogFile(LogType_Info, "GetAlbumMountResult: storage(%d)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumUsage16(sf::Out<AlbumUsage16> usage, StorageId storage, u8 flags) {
        WriteLogFile(LogType_Info, "GetAlbumUsage16: storage(%hhd), flags(%hhd)", storage, flags);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetMinMaxAppletId(sf::OutNonSecureBuffer minMax, sf::Out<bool> success) {
        WriteLogFile(LogType_Info, "GetMinMaxAppletId");
        u64 bufSize = minMax.GetSize();
        if (bufSize != 0) {
            u64 *arr = (u64*)minMax.GetPointer();
            arr[0] = 0x100000000001000;
            arr[1] = 0x100000000001fff;
        }
        success.SetValue(bufSize != 0);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumFileCountEx0(sf::Out<u64> out, StorageId storage, u8 flags) {
        WriteLogFile(LogType_Info, "GetAlbumFileCountEx0: storage(%hhd), flags(%hhd)", storage, flags);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumFileListEx0(sf::OutBuffer buffer, sf::Out<u64> out, u64 storage, u64 flags) {
        WriteLogFile(LogType_Info, "GetAlbumFileListEx0: bufferSize(%ld), storage(%ld), flags(%ld)", buffer.GetSize(), storage, flags);
        return ResultSuccess();
    }

    //Result AlbumAccessorService::SaveEditedScreenShot

    Result AlbumAccessorService::GetLastOverlayScreenShotThumbnail(sf::OutBuffer buffer, sf::Out<CapsOverlayThumbnailData> out) {
        WriteLogFile(LogType_Info, "GetLastOverlayScreenShotThumbnail: bufferSize(%ld)", buffer.GetSize());
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetLastOverlayMovieThumbnail(sf::OutBuffer buffer, sf::Out<CapsOverlayThumbnailData> out) {
        WriteLogFile(LogType_Info, "GetLastOverlayScreenShotThumbnail: bufferSize(%ld)", buffer.GetSize());
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAutoSavingStorage(sf::Out<StorageId> out) {
        WriteLogFile(LogType_Info, "GetAutoSavingStorage");
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetRequiredStorageSpaceSizeToCopyAll(sf::Out<u64> out, StorageId dst, StorageId src) {
        WriteLogFile(LogType_Info, "GetRequiredStorageSpaceSizeToCopyAll: dst(%hhd), src(%hhd)", dst, src);
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImageEx0(sf::Out<CapsScreenShotAttribute> attrs, sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile(LogType_Info, "LoadAlbumScreenShotImageEx0: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotThumbnailImageEx0(attrs.GetPointer(), width.GetPointer(), height.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), fileId, opts);
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImageEx1(sf::Out<LoadAlbumScreenShotImageOutput> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile(LogType_Info, "LoadAlbumScreenShotImageEx0: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotImageEx1(out.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), fileId, opts);
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImageEx1(sf::Out<LoadAlbumScreenShotImageOutput> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile(LogType_Info, "LoadAlbumScreenShotImageEx0: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotThumbnailImageEx1(out.GetPointer(), work.GetPointer(), work.GetSize(), image.GetPointer(), image.GetSize(), fileId, opts);
    }

    Result AlbumAccessorService::ForceAlbumUnmounted(StorageId storage) {
        WriteLogFile(LogType_Info, "ForceAlbumUnmounted: storage(%hhd)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::ResetAlbumMountStatus(StorageId storage) {
        WriteLogFile(LogType_Info, "ResetAlbumMountStatus: storage(%hhd)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::RefreshAlbumCache(StorageId storage) {
        WriteLogFile(LogType_Info, "RefreshAlbumCache: storage(%hhd)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumCache(sf::Out<CapsAlbumCache> cache, StorageId storage) {
        WriteLogFile(LogType_Info, "GetAlbumCache: storage(%hhd)", storage);
        return impl::GetAlbumCache(cache.GetPointer(), storage, ContentType::Screenshot);
    }

    Result AlbumAccessorService::GetAlbumCacheEx(sf::Out<CapsAlbumCache> cache, StorageId storage, const ContentType type) {
        WriteLogFile(LogType_Info, "GetAlbumCacheEx: storage(%hhd), type(%hhd)", storage, type);
        return impl::GetAlbumCache(cache.GetPointer(), storage, type);
    }

    Result AlbumAccessorService::GetAlbumEntryFromApplicationAlbumEntryAruid(sf::Out<CapsAlbumEntry> out, const sf::ClientAppletResourceUserId &aruid, const ApplicationEntry &appEntry) {
        WriteLogFile(LogType_Info, "GetAlbumEntryFromApplicationAlbumEntryAruid: ApplicationEntry(%s), aruid(%ld)", appEntry.AsString().c_str(), aruid.GetValue());
        return ResultSuccess();
    }

    Result AlbumAccessorService::SetInternalErrorConversionEnabled(const bool enabled) {
        WriteLogFile(LogType_Info, "SetInternalErrorConversionEnabled: enabled(%s)", enabled ? "true" : "false");
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadMakerNoteInfoForDebug(sf::Out<u64> out, sf::OutBuffer unk0, sf::OutBuffer unk1, const FileId &fileId) {
        WriteLogFile(LogType_Info, "LoadMakerNoteInfoForDebug: bufferSize(%ld), bufferSize(%ld), fileId(%s)", unk0.GetSize(), unk1.GetSize(), fileId.AsString().c_str());
        return ResultSuccess();
    }

    Result AlbumAccessorService::OpenAccessorSession(sf::Out<std::shared_ptr<AccessorSession>> out, const sf::ClientAppletResourceUserId &aruid) {
        WriteLogFile(LogType_Info, "OpenAccessorSession: pid(%ld), aruid(%ld)", aruid.GetValue(), aruid);
        auto session = std::make_shared<AccessorSession>();
        out.SetValue(std::move(session));
        return ResultSuccess();
    }

}