#pragma once
#include <stratosphere.hpp>
#include "../capsrv_album_settings.hpp"
#include "../capsrv_stream.hpp"
#include "../capsrv_error_converter.hpp"
#include "../capsrv_album_manager.hpp"
#include "../capsrv_file_id_generator.hpp"
#include "../capsrv_overlay_holder.hpp"
#include "../capsrv_application_manager.hpp"

namespace ams::capsrv::server {

    extern u8 g_ZeroHash[ams::crypto::Sha256Generator::HashSize];
    extern AlbumSettings g_AlbumSettings;
    extern StreamIdGenerator g_StreamIdGenerator;
    extern AlbumErrorConverter g_AlbumErrorConverter;
    extern AlbumManager g_AlbumManager;
    extern AlbumFileIdGenerator g_AlbumFileIdGenerator;
    extern OverlayThumbnailHolder g_OverlayThumbnailHolder;
    extern ApplicationAlbumManager g_ApplicationAlbumManager;

    extern os::Mutex g_AlbumMutex;
    extern os::Mutex g_ApplicationMutex;

}
