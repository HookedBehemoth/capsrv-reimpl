#include "capsrv_album_accessor_service.hpp"
#include "capsrv_album_application_service.hpp"
#include "capsrv_album_control_service.hpp"
#include "impl/capsrv_fs.hpp"

extern "C" {
extern u32 __start__;

u32 __nx_applet_type = AppletType_None;

#define INNER_HEAP_SIZE 0x2000
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
ncm::ProgramId CurrentProgramId = {0x0133700000000022ul};
//ncm::ProgramId CurrentProgramId = ncm::ProgramId::CapSrv;

namespace result {

bool CallFatalOnResultAssertion = false;

}

} // namespace ams

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
		R_ASSERT(capsdcInitialize());
		R_ASSERT(fsInitialize());
		R_ASSERT(setsysInitialize());
		R_ASSERT(timeInitialize());
	});

	/* TODO: remove this for a better logging solution (TMA, LogManager?) */
	fsdevMountSdmc();
}

void __appExit(void) {
	/* Cleanup services. */
	timeExit();
	setsysExit();
	fsdevUnmountAll();
	fsExit();
	capsdcExit();
}

// TODO: Custom Init function call?
//static constexpr auto MakeAlbumAccessor = []() { return std::make_shared<capsrv::AlbumAccessorService>(); };
//static constexpr auto MakeAlbumControl = []() { return std::make_shared<capsrv::AlbumControlService>(); };
//static constexpr auto AlbumApplication = []() { return std::make_shared<capsrv::AlbumApplicationService>(); };

namespace {

/* caps:a, caps:c, caps:u. */
constexpr size_t NumServers = 3;
sf::hipc::ServerManager<NumServers> g_server_manager;

constexpr sm::ServiceName AlbumAccessorServiceName = sm::ServiceName::Encode("lmao:a");
constexpr size_t AlbumAccessorMaxSessions = 0x10;

constexpr sm::ServiceName AlbumControlServiceName = sm::ServiceName::Encode("lmao:c");
constexpr size_t AlbumControlMaxSessions = 0x10;

constexpr sm::ServiceName AlbumApplicationServiceName = sm::ServiceName::Encode("lmao:u");
constexpr size_t AlbumApplicationMaxSessions = 0x10;

} // namespace

int main(int argc, char **argv) {
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