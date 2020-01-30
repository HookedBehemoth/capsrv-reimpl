#include "capsrv_album_accessor_service.hpp"
#include "capsrv_album_control_service.hpp"
#include "capsrv_album_application_service.hpp"

#include <stratosphere/sm.hpp>
#include <stratosphere/sf.hpp>
#include <vapours/results.hpp>

extern "C" {
    extern u32 __start__;

    u32 __nx_applet_type = AppletType_None;

    #define INNER_HEAP_SIZE 0x2000
    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char   nx_inner_heap[INNER_HEAP_SIZE];
    
    void __libnx_initheap(void);
    void __appInit(void);
    void __appExit(void);

    /* Exception handling. */
    alignas(16) u8 __nx_exception_stack[ams::os::MemoryBlockUnitSize];
    u64 __nx_exception_stack_size = sizeof(__nx_exception_stack);
    void __libnx_exception_handler(ThreadExceptionDump *ctx);

}

namespace ams {
    ncm::ProgramId CurrentProgramId = { 0x0100013370000022ul };

    namespace result {

        bool CallFatalOnResultAssertion = true;

    }

}

using namespace ams;

void __libnx_initheap(void) {
    void*  addr = nx_inner_heap;
    size_t size = nx_inner_heap_size;

    /* Newlib */
    extern char* fake_heap_start;
    extern char* fake_heap_end;

    fake_heap_start = (char*)addr;
    fake_heap_end   = (char*)addr + size;
}

void __appInit(void) {
    hos::SetVersionForLibnx();

    sm::DoWithSession([] {
        R_ASSERT(fsInitialize());
        R_ASSERT(timeInitialize());
    });

    /* TODO: remove this for a better logging solution (TMA, LogManager?) */
    fsdevMountSdmc();
}

void __appExit(void) {
    /* Cleanup services. */
    
    fsdevUnmountAll();
    fsExit();
}

// TODO: Custom Init function call?
//static constexpr auto MakeAlbumAccessor = []() { return std::make_shared<capsrv::AlbumAccessorService>(); };
//static constexpr auto MakeAlbumControl = []() { return std::make_shared<capsrv::AlbumControlService>(); };
//static constexpr auto AlbumApplication = []() { return std::make_shared<capsrv::AlbumApplicationService>(); };

namespace {

    /* caps:a, caps:c, caps:u. */
    constexpr size_t NumServers  = 1;
    sf::hipc::ServerManager<NumServers> g_server_manager;

    constexpr sm::ServiceName AlbumAccessorServiceName    = sm::ServiceName::Encode("lmao:a");
    constexpr size_t          AlbumAccessorMaxSessions    = 4;

    constexpr sm::ServiceName AlbumControlServiceName     = sm::ServiceName::Encode("lmao:c");
    constexpr size_t          AlbumControlMaxSessions     = 4;

    constexpr sm::ServiceName AlbumApplicationServiceName = sm::ServiceName::Encode("lmao:u");
    constexpr size_t          AlbumApplicationMaxSessions = 4;

}

int main(int argc, char **argv)
{
    /* Create services */
    R_ASSERT(g_server_manager.RegisterServer<capsrv::AlbumAccessorService>(AlbumAccessorServiceName, AlbumAccessorMaxSessions));

    R_ASSERT(g_server_manager.RegisterServer<capsrv::AlbumControlService>(AlbumControlServiceName, AlbumControlMaxSessions));

    if (hos::GetVersion() >= hos::Version_500) {
        R_ASSERT(g_server_manager.RegisterServer<capsrv::AlbumApplicationService>(AlbumApplicationServiceName, AlbumApplicationMaxSessions));
    }

    g_server_manager.LoopProcess();

    return 0;
}