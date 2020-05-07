#include "capsrv_album_object.hpp"

namespace ams::capsrv::server {

    u8 g_ZeroHash[crypto::Sha256Generator::HashSize];

    AlbumSettings g_AlbumSettings;
    StreamIdGenerator g_StreamIdGenerator;
    AlbumErrorConverter g_AlbumErrorConverter;
    AlbumManager g_AlbumManager;
    AlbumFileIdGenerator g_AlbumFileIdGenerator;
    OverlayThumbnailHolder g_OverlayThumbnailHolder;
    ApplicationAlbumManager g_ApplicationAlbumManager;

    os::Mutex g_AlbumMutex(false);
    os::Mutex g_ApplicationMutex(false);

}
