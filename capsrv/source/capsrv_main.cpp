#include "capsrv_crypto.hpp"
#include "capsrv_environment.hpp"
#include "server/capsrv_album_server.hpp"

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

namespace ams {

    ncm::ProgramId CurrentProgramId = ncm::SystemProgramId::CapSrv;

    namespace result {

        bool CallFatalOnResultAssertion = false;

    }

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
    hos::InitializeForStratosphere();

    sm::DoWithSession([] {
        R_ASSERT(setsysInitialize());
        R_ASSERT(splCryptoInitialize());
        R_ASSERT(time::Initialize());
        R_ASSERT(fsInitialize());
        R_ASSERT(capsdcInitialize());
    });
}

void __appExit(void) {
    capsdcExit();
    fsExit();
    timeExit();
    splCryptoExit();
    setsysExit();
}

namespace {

    constexpr size_t ThreadStackSize = 0x8000;
    alignas(os::MemoryPageSize) u8 g_system_thread_stack[ThreadStackSize];
    alignas(os::MemoryPageSize) u8 g_application_thread_stack[ThreadStackSize];

    os::ThreadType g_system_thread, g_application_thread;

}

int main(int argc, char **argv) {
    /* TODO: Initialize StandardAllocator */
    /* Cache environment variables. */
    capsrv::LoadEnvironment();

    os::SetThreadNamePointer(os::GetCurrentThread(), AMS_GET_SYSTEM_THREAD_NAME(capsrv, Main));
    os::ChangeThreadPriority(os::GetCurrentThread(), 0x15);

    /* TODO: Unknown */
    /* TODO: More allocator memes. */

    /* Official software sets the mac generation functions here. */
    R_ABORT_UNLESS(capsrv::crypto::Initialize());

    /* Initialize all internal modules. */
    capsrv::server::Initialize();

    /* Start sytem service thread. */
    R_ABORT_UNLESS(os::CreateThread(std::addressof(g_system_thread), capsrv::server::AlbumControlServerThreadFunction, nullptr, g_system_thread_stack, ThreadStackSize, AMS_GET_SYSTEM_THREAD_PRIORITY(capsrv, SystemIpcServer)));
    os::StartThread(std::addressof(g_system_thread));
    os::SetThreadNamePointer(std::addressof(g_system_thread), AMS_GET_SYSTEM_THREAD_NAME(capsrv, SystemIpcServer));

    /* Start application service thread. */
    R_ABORT_UNLESS(os::CreateThread(std::addressof(g_application_thread), capsrv::server::AlbumServerThreadFunction, nullptr, g_application_thread_stack, ThreadStackSize, AMS_GET_SYSTEM_THREAD_PRIORITY(capsrv, ApplicationIpcServer)));
    os::StartThread(std::addressof(g_application_thread));
    os::SetThreadNamePointer(std::addressof(g_application_thread), AMS_GET_SYSTEM_THREAD_NAME(capsrv, ApplicationIpcServer));

    /* Wait for our threads. */
    os::WaitThread(std::addressof(g_system_thread));
    os::WaitThread(std::addressof(g_application_thread));

    /* Cleanup. */
    capsrv::server::Exit();
    return 0;
}
