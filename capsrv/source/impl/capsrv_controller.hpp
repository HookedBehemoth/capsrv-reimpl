#pragma once
#include "../capsrv_types.hpp"

namespace ams::capsrv::control {

    Result SetShimLibraryVersion(u64 version, u64 aruid);

    Result RegisterAppletResourceUserId(u64 aruid, u64 appId);
    Result UnregisterAppletResourceUserId(u64 aruid, u64 appId);

    Result GetApplicationIdFromAruid(u64 *appId, u64 aruid);
    Result CheckApplicationIdRegistered(u64 appId);

    Result GenerateApplicationAlbumEntry(ApplicationEntry *appEntry, const Entry &entry, u64 appId);

    Result EncryptAafeBuffer(ApplicationFileEntry *appFileEntry, u64 count, u64 aruid);
    Result EncryptAaeBuffer(ApplicationEntry *appEntry, u64 count, u64 aruid);

    Result GetAlbumEntryFromApplicationAlbumEntry(Entry *out, const ApplicationEntry *src, u64 applicationId);
    Result GetAlbumEntryFromApplicationAlbumEntryAruid(Entry *out, const ApplicationEntry *src, u64 aruid);

}