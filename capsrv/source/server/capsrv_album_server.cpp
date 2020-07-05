#include "capsrv_album_server.hpp"

#include "capsrv_album_object.hpp"
#include "../hipc/capsrv_album_accessor_service.hpp"
#include "../hipc/capsrv_album_application_service.hpp"
#include "../hipc/capsrv_album_control_service.hpp"

namespace ams::capsrv::server {

    namespace {

        /* System services. */

        constexpr size_t AlbumSystemNumServers = 1;
        sf::hipc::ServerManager<AlbumSystemNumServers> g_AlbumControlServerManager;

        constexpr sm::ServiceName AlbumControlServiceName = sm::ServiceName::Encode("caps:c");
        constexpr size_t AlbumControlMaxSessions = 0x10;

        /* Application services. */

        constexpr size_t AlbumApplicationNumServers = 2;
        sf::hipc::ServerManager<AlbumApplicationNumServers> g_AlbumServerManager;

        constexpr sm::ServiceName AlbumAccessorServiceName = sm::ServiceName::Encode("caps:a");
        constexpr size_t AlbumAccessorMaxSessions = 0x10;

        constexpr sm::ServiceName AlbumApplicationServiceName = sm::ServiceName::Encode("caps:u");
        constexpr size_t AlbumApplicationMaxSessions = 0x10;

        bool IsInitialized = false;

        constexpr const size_t ManagerMemorySize = 0x51000;
        constexpr const size_t OverlayMemorySize = 0xC000;
        constexpr const size_t TotalMemorySize = OverlayMemorySize + ManagerMemorySize;

        alignas(0x1000) u8 g_AlbumWorkMemory[TotalMemorySize];
        constexpr u8 *g_ManagerMemory = g_AlbumWorkMemory;
        constexpr u8 *g_OverlayMemory = g_AlbumWorkMemory + ManagerMemorySize;

        Result InitializeAlbumControlServerServer() {
            R_ABORT_UNLESS(g_AlbumControlServerManager.RegisterServer<AlbumControlService>(AlbumControlServiceName, AlbumControlMaxSessions));
            return ResultSuccess();
        }

        Result InitializeAlbumServerServer() {
            R_ABORT_UNLESS(g_AlbumServerManager.RegisterServer<AlbumAccessorService>(AlbumAccessorServiceName, AlbumAccessorMaxSessions));

            if (hos::GetVersion() >= hos::Version_5_0_0) {
                R_ABORT_UNLESS(g_AlbumServerManager.RegisterServer<AlbumApplicationService>(AlbumApplicationServiceName, AlbumApplicationMaxSessions));
            }

            return ResultSuccess();
        }

    }

    Result Initialize() {
        AMS_ABORT_UNLESS(!IsInitialized);

        std::memset(reinterpret_cast<void *>(g_AlbumWorkMemory), 0, TotalMemorySize);
        g_AlbumSettings.Initialize();
        g_StreamIdGenerator.Initialize();
        g_AlbumErrorConverter.Initialize();

        /* Hash over empty memory segment to compare memory against. */
        crypto::GenerateSha256Hash(g_ZeroHash, crypto::Sha256Generator::HashSize, g_AlbumWorkMemory, 0x40000);
        g_AlbumManager.Initialize(&g_AlbumSettings, &g_StreamIdGenerator);
        g_AlbumManager.SetMemory(g_ManagerMemory, ManagerMemorySize);
        g_AlbumFileIdGenerator.Initialize();
        g_OverlayThumbnailHolder.SetMemory(g_OverlayMemory, 0xC000);
        g_OverlayThumbnailHolder.Initialize();
        g_ApplicationAlbumManager.Initialize();

        R_ABORT_UNLESS(InitializeAlbumControlServerServer());
        R_ABORT_UNLESS(InitializeAlbumServerServer());

        g_AlbumManager.MountImageDirectory(StorageId::Nand);
        g_AlbumManager.RefreshCache(StorageId::Nand);

        IsInitialized = true;

        return ResultSuccess();
    }

    void Exit() {
        AMS_ABORT_UNLESS(IsInitialized);

        /* <Service Cleanup> */
        g_ApplicationAlbumManager.Exit();
        g_OverlayThumbnailHolder.Exit();
        g_AlbumFileIdGenerator.Exit();
        g_AlbumManager.Exit();
        /* The hash has an 'Exit' function... */
        g_AlbumErrorConverter.Exit();
        g_StreamIdGenerator.Exit();
        g_AlbumSettings.Exit();

        IsInitialized = false;
    }

    void AlbumControlServerThreadFunction(void *) {
        AMS_ABORT_UNLESS(IsInitialized);

        g_AlbumControlServerManager.LoopProcess();
    }

    void AlbumServerThreadFunction(void *) {
        AMS_ABORT_UNLESS(IsInitialized);

        g_AlbumServerManager.LoopProcess();
    }

}
