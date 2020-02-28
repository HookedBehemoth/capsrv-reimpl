#include "capsrv_config.hpp"
#include "capsrv_crypto.hpp"
#include "image/exif_extractor.hpp"
#include "impl/capsrv_controller.hpp"
#include "impl/capsrv_manager.hpp"
#include "impl/capsrv_overlay.hpp"
#include "logger.hpp"

namespace ams {

    ncm::ProgramId CurrentProgramId = ncm::ProgramId::CapSrv;

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

#define INNER_HEAP_SIZE 0x80000
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
    fsdevMountSdmc();
#endif /* __DEBUG__ */
}

void __appExit(void) {
#ifdef __DEBUG__
    fsdevUnmountAll();
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

#define RUNN(function)                                \
    {                                                 \
        u32 rc = function;                            \
        if (R_FAILED(rc))                             \
            printf("0x%x %s\n", function, #function); \
    }

using namespace ams::capsrv;

#define RUN(function)                                                       \
    {                                                                       \
        ams::Result rc = function;                                          \
        if (R_FAILED(rc))                                                   \
            printf("0x%x %s expected success\n", rc.GetValue(), #function); \
    }

#define FAIL(function)                                                      \
    {                                                                       \
        ams::Result rc = function;                                          \
        if (R_SUCCEEDED(rc))                                                \
            printf("0x%x %s expected failure\n", rc.GetValue(), #function); \
    }

#define TEST(function, var, expected)                                    \
    {                                                                    \
        RUN(function)                                                    \
        if (var != expected)                                             \
            printf("FAILED %s: %s != %s\n", #function, #var, #expected); \
    }

int main(int argc, char **argv) {
    int sock = nxlinkStdio();

    config::Initialize();
    R_ASSERT(crypto::Initialize());
    ovl::Initialize();

    RUN(impl::MountAlbum(StorageId::Nand));
    RUN(impl::MountAlbum(StorageId::Sd));

    u64 count;

    TEST(impl::GetAlbumFileCount(&count, StorageId::Nand, CapsAlbumFileContentsFlag_ScreenShot), count, 9);
    TEST(impl::GetAlbumFileCount(&count, StorageId::Sd, CapsAlbumFileContentsFlag_ScreenShot), count, 5);
    TEST(impl::GetAlbumFileCount(&count, StorageId::Nand, CapsAlbumFileContentsFlag_Movie), count, 0);
    TEST(impl::GetAlbumFileCount(&count, StorageId::Sd, CapsAlbumFileContentsFlag_Movie), count, 0);
    TEST(impl::GetAlbumFileCount(&count, StorageId::Nand, CapsAlbumFileContentsFlag_ScreenShot | CapsAlbumFileContentsFlag_Movie), count, 9);
    TEST(impl::GetAlbumFileCount(&count, StorageId::Sd, CapsAlbumFileContentsFlag_ScreenShot | CapsAlbumFileContentsFlag_Movie), count, 5);

    Entry entries[10] = {0};

    TEST(impl::GetAlbumFileList(entries, 10, &count, StorageId::Nand, CapsAlbumFileContentsFlag_ScreenShot), count, 9);
    TEST(impl::GetAlbumFileList(entries, 10, &count, StorageId::Sd, CapsAlbumFileContentsFlag_ScreenShot), count, 5);

    Entry &ent = entries[0];
    printf("%s\n", ent.AsString().c_str());

    u64 size = ent.size;
    u8 *buffer = (u8 *)malloc(size);
    u64 readSize = 0;
    RUN(impl::LoadAlbumFile(buffer, size, &readSize, ent.fileId));
    printf("%ld\n", readSize);
    for (int i = 0; i < 0x10; i++)
        printf("%02X", buffer[i]);
    printf("\n");

    RUN(impl::LoadAlbumFileThumbnail(buffer, size, &readSize, ent.fileId));
    printf("%ld\n", readSize);
    for (int i = 0; i < 0x10; i++)
        printf("%02X", buffer[i]);
    printf("\n");

    u64 thumbSize = 320*180*4;
    u8 *img = (u8 *)malloc(thumbSize);
    u64 width = 0, height = 0;
    RUN(impl::LoadAlbumScreenShotThumbnailImage(&width, &height, img, thumbSize, buffer, readSize, ent.fileId));
    printf("%ld:%ld\n", width, height);
    for (int i = 0; i < 0x10; i++)
        printf("%02X", buffer[i]);
    printf("\n");
    for (int i = 0; i < 0x10; i++)
        printf("%02X", img[i]);
    printf("\n");

    /* Can't test exact equlity since datetime is... time. */
    FileId fileId = {0};
    RUN(control::GenerateCurrentAlbumFileId(&fileId, 0x1337, ContentType::Screenshot));
    RUN(control::GenerateCurrentAlbumFileId(&fileId, 0x1337, ContentType::Movie));

    FAIL(control::CheckApplicationIdRegistered(0x1337));
    RUN(control::RegisterAppletResourceUserId(0x420, 0x1337));
    RUN(control::CheckApplicationIdRegistered(0x1337));
    u64 appId;
    TEST(control::GetApplicationIdFromAruid(&appId, 0x420), appId, 0x1337);

    Entry entry = {0x1337, fileId};
    Entry exp = entry;
    ApplicationEntry appEntry;
    RUN(control::GenerateApplicationAlbumEntry(&appEntry, entry, 0x1337));
    TEST(control::GetAlbumEntryFromApplicationAlbumEntry(&exp, &appEntry, 0x1337), exp, entry);

    FAIL(control::SetShimLibraryVersion(0, 0x420));
    RUN(control::SetShimLibraryVersion(1, 0x420));
    RUN(control::GenerateApplicationAlbumEntry(&appEntry, entry, 0x1337));
    TEST(control::GetAlbumEntryFromApplicationAlbumEntry(&entry, &appEntry, 0x1337), entry, exp);
    TEST(control::GetAlbumEntryFromApplicationAlbumEntryAruid(&entry, &appEntry, 0x420), entry, exp);

    RUN(control::UnregisterAppletResourceUserId(0x420, 0x1337));

    RUN(impl::UnmountAlbum(StorageId::Nand));
    RUN(impl::UnmountAlbum(StorageId::Sd));

    config::Exit();

    printf("\n\nfinished\n\n\n");

    close(sock);

    return 0;
}
#else
    static_assert(false, "undefined state");
#endif
