#include "capsrv_config.hpp"
#include "capsrv_crypto.hpp"
#include "capsrv_time.hpp"
#include "image/exif_extractor.hpp"
#include "impl/capsrv_controller.hpp"
#include "impl/capsrv_file_id_generator.hpp"
#include "impl/capsrv_manager.hpp"
#include "impl/capsrv_overlay.hpp"
#include "logger.hpp"

namespace ams {

    ncm::ProgramId CurrentProgramId = ncm::SystemProgramId::CapSrv;

    namespace result {

        bool CallFatalOnResultAssertion = false;

    }

}

#ifdef SYSTEM_MODULE

#include "hipc/capsrv_album_accessor_service.hpp"
#include "hipc/capsrv_album_application_service.hpp"
#include "hipc/capsrv_album_control_service.hpp"

extern "C" {
extern u32 __start__;

u32 __nx_applet_type = AppletType_None;
TimeServiceType __nx_time_service_type = TimeServiceType_User;

#define INNER_HEAP_SIZE 0x8000
size_t nx_inner_heap_size = INNER_HEAP_SIZE;
char nx_inner_heap[INNER_HEAP_SIZE];

void __libnx_initheap(void);
void __appInit(void);
void __appExit(void);

/* Exception handling. */
alignas(16) u8 __nx_exception_stack[ams::os::MemoryPageSize];
u64 __nx_exception_stack_size = sizeof(__nx_exception_stack);
void __libnx_exception_handler(ThreadExceptionDump *ctx);
}

using namespace ams;

void __libnx_initheap(void) {
    void *addr = nx_inner_heap;
    size_t size = nx_inner_heap_size;

    /* Newlib */
    extern char *fake_heap_start;
    extern char *fake_heap_end;

    fake_heap_start = (char *)addr;
    fake_heap_end = (char *)addr + size;
}

void __appInit(void) {
    hos::SetVersionForLibnx();

    sm::DoWithSession([] {
        R_ASSERT(setsysInitialize());
        R_ASSERT(splCryptoInitialize());
        R_ASSERT(timeInitialize());
        R_ASSERT(fsInitialize());
        R_ASSERT(capsdcInitialize());
    });

    /* TODO: remove this for a better logging solution (TMA, LogManager?) */
#ifdef __DEBUG__
    LogInit();
#endif /* __DEBUG__ */
}

void __appExit(void) {
#ifdef __DEBUG__
    LogExit();
#endif /* __DEBUG__ */
    /* Cleanup services. */
    capsdcExit();
    fsExit();
    timeExit();
    splCryptoExit();
    setsysExit();
}

namespace {

    /* caps:a, caps:c, caps:u. */
    constexpr size_t NumServers = 3;
    sf::hipc::ServerManager<NumServers> g_server_manager;

    constexpr sm::ServiceName AlbumAccessorServiceName = sm::ServiceName::Encode("caps:a");
    constexpr size_t AlbumAccessorMaxSessions = 0x10;

    constexpr sm::ServiceName AlbumControlServiceName = sm::ServiceName::Encode("caps:c");
    constexpr size_t AlbumControlMaxSessions = 0x10;

    constexpr sm::ServiceName AlbumApplicationServiceName = sm::ServiceName::Encode("caps:u");
    constexpr size_t AlbumApplicationMaxSessions = 0x10;

}

int main(int argc, char **argv) {
    capsrv::time::Initialize();
    capsrv::config::Initialize();
    R_ASSERT(capsrv::crypto::Initialize());
    capsrv::ovl::Initialize();

    capsrv::impl::MountAlbum(capsrv::StorageId::Nand);

    /* Create services */
    R_ASSERT(g_server_manager.RegisterServer<capsrv::AlbumAccessorService>(AlbumAccessorServiceName, AlbumAccessorMaxSessions));

    R_ASSERT(g_server_manager.RegisterServer<capsrv::AlbumControlService>(AlbumControlServiceName, AlbumControlMaxSessions));

    if (hos::GetVersion() >= hos::Version_500) {
        R_ASSERT(g_server_manager.RegisterServer<capsrv::AlbumApplicationService>(AlbumApplicationServiceName, AlbumApplicationMaxSessions));
    }

    g_server_manager.LoopProcess();

    capsrv::impl::UnmountAlbum(capsrv::StorageId::Nand);
    capsrv::impl::UnmountAlbum(capsrv::StorageId::Sd);

    return 0;
}

#elif APPLET_TEST

extern "C" {
void __appInit(void);
void __appExit(void);
}

void __appInit(void) {
    ams::sm::DoWithSession([] {
        R_ASSERT(setsysInitialize());

        SetSysFirmwareVersion fw;
        R_ASSERT(setsysGetFirmwareVersion(&fw));
        hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));

        R_ASSERT(appletInitialize());
        R_ASSERT(timeInitialize());
        R_ASSERT(fsInitialize());
        R_ASSERT(splCryptoInitialize());
        R_ASSERT(capsdcInitialize());

        R_ASSERT(socketInitializeDefault());
    });
}

void __appExit(void) {
    socketExit();
    /* Cleanup services. */
    capsdcExit();
    splCryptoExit();
    fsExit();
    timeExit();
    appletExit();
    setsysExit();
}

using namespace ams::capsrv;

#define RUN(function)                                                       \
    ({                                                                      \
        auto const &rc = (function);                                        \
        if (R_FAILED(rc))                                                   \
            printf("0x%x %s expected success\n", rc.GetValue(), #function); \
    })

#define FAIL(function)                                                      \
    ({                                                                      \
        auto const &rc = (function);                                        \
        if (R_SUCCEEDED(rc))                                                \
            printf("0x%x %s expected failure\n", rc.GetValue(), #function); \
    })

#define TEST(function, var, expected)                                    \
    ({                                                                   \
        RUN(function);                                                   \
        if (var != expected)                                             \
            printf("FAILED %s: %s != %s\n", #function, #var, #expected); \
    })

int main(int argc, char **argv) {
    int sock = nxlinkStdio();

    time::Initialize();
    config::Initialize();
    R_ASSERT(crypto::Initialize());
    ovl::Initialize();

    RUN(impl::MountAlbum(StorageId::Nand));
    RUN(impl::MountAlbum(StorageId::Sd));

    u64 count;

    TEST(impl::GetAlbumFileCount(&count, StorageId::Nand, CapsAlbumFileContentsFlag_ScreenShot), count, 6);
    TEST(impl::GetAlbumFileCount(&count, StorageId::Sd, CapsAlbumFileContentsFlag_ScreenShot), count, 9);
    TEST(impl::GetAlbumFileCount(&count, StorageId::Nand, CapsAlbumFileContentsFlag_Movie), count, 0);
    TEST(impl::GetAlbumFileCount(&count, StorageId::Sd, CapsAlbumFileContentsFlag_Movie), count, 3);
    TEST(impl::GetAlbumFileCount(&count, StorageId::Nand, CapsAlbumFileContentsFlag_ScreenShot | CapsAlbumFileContentsFlag_Movie), count, 6);
    TEST(impl::GetAlbumFileCount(&count, StorageId::Sd, CapsAlbumFileContentsFlag_ScreenShot | CapsAlbumFileContentsFlag_Movie), count, 12);

    Entry entries[10] = {0};

    TEST(impl::GetAlbumFileList(entries, 10, &count, StorageId::Nand, CapsAlbumFileContentsFlag_ScreenShot), count, 6);
    TEST(impl::GetAlbumFileList(entries, 10, &count, StorageId::Sd, CapsAlbumFileContentsFlag_ScreenShot), count, 9);

    char path[EXTRA_PATH_LENGTH];
    entries[0].fileId.GetFilePath(path, EXTRA_PATH_LENGTH);
    printf("path: %s, size: %ld\n", path, entries[0].size);

    /* Can't test exact equlity since datetime is... time. */
    FileId fileId = {0};
    u64 appId = 0x0100152000022000;
    u64 aruid = 0x420;
    RUN(impl::GenerateCurrentAlbumFileId(&fileId, appId, ContentType::Screenshot));
    printf("%s\n", fileId.AsString());
    RUN(impl::GenerateCurrentAlbumFileId(&fileId, appId, ContentType::Movie));
    printf("%s\n", fileId.AsString());

    FAIL(control::CheckApplicationIdRegistered(appId));
    RUN(control::RegisterAppletResourceUserId(aruid, appId));
    RUN(control::CheckApplicationIdRegistered(appId));
    u64 tmpId;
    TEST(control::GetApplicationIdFromAruid(&tmpId, aruid), tmpId, appId);

    Entry entry = {0x1337, fileId};
    Entry exp = entry;
    ApplicationEntry appEntry;
    RUN(control::GenerateApplicationAlbumEntry(&appEntry, entry, appId));
    TEST(control::GetAlbumEntryFromApplicationAlbumEntry(&exp, &appEntry, appId), exp, entry);

    FAIL(control::SetShimLibraryVersion(0, aruid));
    RUN(control::SetShimLibraryVersion(1, aruid));

    RUN(control::GenerateApplicationAlbumEntry(&appEntry, entry, appId));
    TEST(control::GetAlbumEntryFromApplicationAlbumEntry(&entry, &appEntry, appId), entry, exp);
    TEST(control::GetAlbumEntryFromApplicationAlbumEntryAruid(&entry, &appEntry, aruid), entry, exp);

    RUN(control::UnregisterAppletResourceUserId(aruid, appId));

    RUN(impl::UnmountAlbum(StorageId::Nand));
    RUN(impl::UnmountAlbum(StorageId::Sd));

    config::Exit();

    printf("\nfinished\n");

    close(sock);

    return 0;
}
#else
static_assert(false, "undefined state");
#endif
