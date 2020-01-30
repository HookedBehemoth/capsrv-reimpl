#include "capsrv_album_accessor_service.hpp"
#include "impl/capsrv_manager.hpp"

#include "logger.hpp"

namespace ams::capsrv {

    Result AlbumAccessorService::GetAlbumFileCount(sf::Out<u64> out, const StorageId storage) {
        WriteLogFile(LogType_Info, "GetAlbumFileCount: storage(%hhd)", storage);
        return impl::GetAlbumFileCount(out.GetPointer(), storage);
    }

    Result AlbumAccessorService::GetAlbumFileList(sf::OutBuffer buffer, sf::Out<u64> out, const StorageId storage) {
        WriteLogFile(LogType_Info, "GetAlbumFileList: storage(%hhd), bufferSize(%ld)", storage, buffer.GetSize());
        return impl::GetAlbumFileList(buffer.GetPointer(), buffer.GetSize(), out.GetPointer(), storage);
    }

    Result AlbumAccessorService::LoadAlbumFile(sf::OutBuffer buffer, sf::Out<u64> out, const FileId &fileId) {
        WriteLogFile(LogType_Info, "LoadAlbumFile: fileId(%s), bufferSize(%ld)", fileId.AsString().c_str(), buffer.GetSize());
        return impl::LoadAlbumFile(buffer.GetPointer(), buffer.GetSize(), out.GetPointer(), fileId);
    }

    Result AlbumAccessorService::DeleteAlbumFile(const FileId &fileId) {
        WriteLogFile(LogType_Info, "DeleteAlbumFile: fileId(%s)", fileId.AsString().c_str());
        return ResultSuccess();
    }

    Result AlbumAccessorService::StorageCopyAlbumFile(const StorageId storage, const FileId &fileId) {
        WriteLogFile(LogType_Info, "StorageCopyAlbumFile: fileId(%s), storage(%hhd)", fileId.AsString().c_str(), storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::IsAlbumMounted(sf::Out<bool> out, const StorageId storage) {
        WriteLogFile(LogType_Info, "IsAlbumMounted: storage(%hhd)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumUsage(sf::Out<CapsAlbumUsage2> out, const StorageId storage) {
        WriteLogFile(LogType_Info, "GetAlbumUsage: storage(%hhd)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumFileSize(sf::Out<u64> out, const FileId &fileId) {
        WriteLogFile(LogType_Info, "GetAlbumFileSize: fileId(%s)", fileId.AsString().c_str());
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumFileThumbnail(sf::OutBuffer image, sf::Out<u64> out, const FileId &fileId) {
        WriteLogFile(LogType_Info, "LoadAlbumFileThumbnail: imgSize(%ld), fileId(%s)", image.GetSize(), fileId.AsString().c_str());
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImage(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId) {
        WriteLogFile(LogType_Info, "LoadAlbumScreenShotImage: workSize(%ld), imgSize(%ld), fileId(%s)", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImage(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId) {
        WriteLogFile(LogType_Info, "LoadAlbumScreenShotThumbnailImage: workSize(%ld), imgSize(%ld), fileId(%s)", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        CapsScreenShotDecodeOption opts = {0};
        return impl::LoadAlbumScreenShotThumbnail(width.GetPointer(), height.GetPointer(), nullptr, nullptr, nullptr, nullptr, image.GetPointer(), image.GetSize(), work.GetPointer(), work.GetSize(), fileId, opts);
    }

    Result AlbumAccessorService::GetAlbumEntryFromApplicationAlbumEntry(sf::Out<CapsAlbumEntry> out, const ApplicationEntry &appEntry, u64 appId) {
        WriteLogFile(LogType_Info, "GetAlbumEntryFromApplicationAlbumEntry: appEntry(%s), appId(%016lx)", appEntry.AsString().c_str(), appId);
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImageEx(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile(LogType_Info, "LoadAlbumScreenShotImageEx: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImageEx(sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile(LogType_Info, "LoadAlbumScreenShotThumbnailImageEx: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotThumbnail(width.GetPointer(), height.GetPointer(), nullptr, nullptr, nullptr, nullptr, image.GetPointer(), image.GetSize(), work.GetPointer(), work.GetSize(), fileId, opts);
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImageEx0(sf::Out<CapsScreenShotAttribute> attrs, sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile(LogType_Info, "LoadAlbumScreenShotImageEx0: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumUsage3(sf::Out<CapsAlbumUsage3> usage, const StorageId storage) {
        WriteLogFile(LogType_Info, "GetAlbumUsage3: storage(%d)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumMountResult(const StorageId storage) {
        WriteLogFile(LogType_Info, "GetAlbumMountResult: storage(%d)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumUsage16(sf::Out<AlbumUsage16> usage, const StorageId storage, const u8 flags) {
        WriteLogFile(LogType_Info, "GetAlbumUsage16: storage(%hhd), flags(%hhd)", storage, flags);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetMinMaxAppletId(sf::OutNonSecureBuffer minMax, sf::Out<bool> success) {
        WriteLogFile(LogType_Info, "GetMinMaxAppletId");
        u64 bufSize = minMax->GetSize();
        if (bufSize != 0) {
            u64[2] arr = minMax->GetPointer();
            arr[0] = 0x100000000001000;
            arr[1] = 0x100000000001fff;
        }
        success.SetValue(buf_size != 0);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumFileCountEx0(sf::Out<u64> out, const StorageId storage, const u8 flags) {
        WriteLogFile(LogType_Info, "GetAlbumFileCountEx0: storage(%hhd), flags(%hhd)", storage, flags);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumFileListEx0(sf::OutBuffer buffer, sf::Out<u64> out, const u64 storage, const u64 flags) {
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

    Result AlbumAccessorService::GetRequiredStorageSpaceSizeToCopyAll(sf::Out<u64> out, const StorageId dst, const StorageId src) {
        WriteLogFile(LogType_Info, "GetRequiredStorageSpaceSizeToCopyAll: dst(%hhd), src(%hhd)", dst, src);
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImageEx0(sf::Out<CapsScreenShotAttribute> attrs, sf::Out<u64> width, sf::Out<u64> height, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile(LogType_Info, "LoadAlbumScreenShotImageEx0: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return impl::LoadAlbumScreenShotThumbnail(width.GetPointer(), height.GetPointer(), attrs.GetPointer(), nullptr, nullptr, nullptr, image.GetPointer(), image.GetSize(), work.GetPointer(), work.GetSize(), fileId, opts);
    }

    Result AlbumAccessorService::LoadAlbumScreenShotImageEx1(sf::Out<LoadAlbumScreenShotImageOutput> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile(LogType_Info, "LoadAlbumScreenShotImageEx0: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        return ResultSuccess();
    }

    Result AlbumAccessorService::LoadAlbumScreenShotThumbnailImageEx1(sf::Out<LoadAlbumScreenShotImageOutput> out, sf::OutNonSecureBuffer work, sf::OutBuffer image, const FileId &fileId, const CapsScreenShotDecodeOption &opts) {
        WriteLogFile(LogType_Info, "LoadAlbumScreenShotImageEx0: workSize(%ld), imgSize(%ld), fileId(%s), opts()", work.GetSize(), image.GetSize(), fileId.AsString().c_str());
        Dimensions dim = {0};
        CapsScreenShotAttribute attr = {0};
        LoadAlbumScreenShotImageOutput *raw = out.GetPointer();
        Result res = impl::LoadAlbumScreenShotThumbnail(&dim.width, &dim.height, &attr, raw->unk_x50, nullptr, nullptr, image.GetPointer(), image.GetSize(), work.GetPointer(), work.GetSize(), fileId, opts);
        if (res.IsSuccess() && raw) {
            raw->width = dim.width;
            raw->height = dim.height;
            raw->attr = attr;
        }
        std::memset(work.GetPointer(), 0, work.GetSize());
        return res;
    }

    Result AlbumAccessorService::ForceAlbumUnmounted(const StorageId storage) {
        WriteLogFile(LogType_Info, "ForceAlbumUnmounted: storage(%hhd)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::ResetAlbumMountStatus(const StorageId storage) {
        WriteLogFile(LogType_Info, "ResetAlbumMountStatus: storage(%hhd)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::RefreshAlbumCache(const StorageId storage) {
        WriteLogFile(LogType_Info, "RefreshAlbumCache: storage(%hhd)", storage);
        return ResultSuccess();
    }

    Result AlbumAccessorService::GetAlbumCache(sf::Out<CapsAlbumCache> cache, const StorageId storage) {
        WriteLogFile(LogType_Info, "GetAlbumCache: storage(%hhd)", storage);
        return impl::GetAlbumCache(cache.GetPointer(), storage, ContentType::Screenshot);
    }

    Result AlbumAccessorService::GetAlbumCacheEx(sf::Out<CapsAlbumCache> cache, const StorageId storage, const ContentType type) {
        WriteLogFile(LogType_Info, "GetAlbumCacheEx: storage(%hhd), type(%hhd)", storage, type);
        return impl::GetAlbumCache(cache.GetPointer(), storage, type);
    }

    //Result AlbumAccessorService::GetAlbumEntryFromApplicationAlbumEntryAruid(sf::Out<CapsAlbumEntry> out, const ApplicationEntry &appEntry, u64 aruid, const sf::ClientProcessId &client_pid)

    Result AlbumAccessorService::SetInternalErrorConversionEnabled(const bool enabled) {
        WriteLogFile(LogType_Info, "SetInternalErrorConversionEnabled: enabled(%s)", enabled ? "true" : "false");
        return ResultSuccess();
    }

    //Result AlbumAccessorService::LoadMakerNoteInfoForDebug(sf::Out<u64> out, sf::OutBuffer unk0, sf::OutBuffer unk1, const FileId &fileId)
    //Result AlbumAccessorService::OpenAccessorSession(sf::Out<std::shared_ptr<IContentStorage>> out, StorageId storage_id)
}