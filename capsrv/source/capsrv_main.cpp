#include "capsrv_config.hpp"
#include "capsrv_crypto.hpp"
#include "impl/capsrv_controller.hpp"
#include "impl/capsrv_fs.hpp"
#include "impl/capsrv_overlay.hpp"

#include "image/exif_extractor.hpp"

//extern "C" {
//extern u32 __start__;
//
//u32 __nx_applet_type = AppletType_Application;
//
//#define INNER_HEAP_SIZE 0x2000
//size_t nx_inner_heap_size = INNER_HEAP_SIZE;
//char nx_inner_heap[INNER_HEAP_SIZE];
//
//void __libnx_initheap(void);
//void __appInit(void);
//void __appExit(void);
//void __libnx_init_time(void);
//
///* Exception handling. */
//alignas(16) u8 __nx_exception_stack[0x1000];
//u64 __nx_exception_stack_size = sizeof(__nx_exception_stack);
//void __libnx_exception_handler(ThreadExceptionDump *ctx);
//}

namespace ams::result {

bool CallFatalOnResultAssertion = false;

} // namespace ams::result

using namespace ams;

//void __libnx_initheap(void) {
//	void *addr = nx_inner_heap;
//	size_t size = nx_inner_heap_size;
//
//	/* Newlib */
//	extern char *fake_heap_start;
//	extern char *fake_heap_end;
//
//	fake_heap_start = (char *)addr;
//	fake_heap_end = (char *)addr + size;
//}
//
//void __appInit(void) {
//	hosversionSet(MAKEHOSVERSION(9, 1, 0));
//	//hos::SetVersionForLibnx();
//
//	//sm::DoWithSession([] {
//	R_ASSERT(smInitialize());
//	R_ASSERT(setsysInitialize());
//	R_ASSERT(timeInitialize());
//	__libnx_init_time();
//	R_ASSERT(fsInitialize());
//	R_ASSERT(capsdcInitialize());
//	smExit();
//	//});
//}
//
//void __appExit(void) {
//	/* Cleanup services. */
//	capsdcExit();
//	fsdevUnmountAll();
//	fsExit();
//	timeExit();
//	setsysExit();
//}

using namespace ams::capsrv;

int main(int argc, char **argv) {
	socketInitializeDefault();
	int sock = nxlinkStdio();
	setsysInitialize();

	config::Initialize();
	config::print();
	printf("0x%x spl\n", splCryptoInitialize());
	printf("0x%x crypto\n", crypto::Initialize().GetValue());

	ovl::Initialize();

	printf("0x%x mount nand\n", impl::MountAlbum(StorageId::Nand).GetValue());
	printf("0x%x mount sd\n", impl::MountAlbum(StorageId::Sd).GetValue());

	u64 count;
	printf("0x%x count nand\n", impl::GetAlbumFileCount(&count, StorageId::Nand, CapsAlbumFileContentsFlag_ScreenShot | CapsAlbumFileContentsFlag_Movie).GetValue());
	printf("%ld\n", count);
	printf("0x%x count sd\n", impl::GetAlbumFileCount(&count, StorageId::Sd, CapsAlbumFileContentsFlag_ScreenShot | CapsAlbumFileContentsFlag_Movie).GetValue());
	printf("%ld\n", count);

	Entry entries[10] = {0};
	printf("0x%x list nand\n", impl::GetAlbumFileList(entries, 2, &count, StorageId::Nand, CapsAlbumFileContentsFlag_ScreenShot).GetValue());
	printf("%s\n", entries[0].AsString().c_str());
	printf("%s\n", entries[1].AsString().c_str());
	printf("%ld\n", count);
	printf("0x%x list sd\n", impl::GetAlbumFileList(entries, 2, &count, StorageId::Sd, CapsAlbumFileContentsFlag_ScreenShot).GetValue());
	printf("%s\n", entries[0].AsString().c_str());
	printf("%s\n", entries[1].AsString().c_str());
	printf("%ld\n", count);

	FILE *f = fopen(entries[1].fileId.GetFilePath().c_str(), "rb");
	if (f) {
		printf("open succ\n");
		fseek(f, 0, SEEK_END);
		auto fsize = ftell(f);
		printf("size: %ld\n", fsize);
		fseek(f, 0, SEEK_SET);
		u8* img = (u8 *)malloc(fsize);
    	fread(img, 1, fsize, f);
		fclose(f);
		printf("closed\n");

		auto bin = ams::image::detail::ExifBinary();
		auto exif = ams::image::ExifExtractor(&bin);

		exif.SetExifData(img + 0xc, fsize - 0xc);
		printf("0x%x\n", exif.Analyse());

		u32 size = 0;
		const char *dateStr = exif.ExtractDateTime(&size);
		if (dateStr) {
			printf("%s\n", dateStr);
			printf("size: %d\n", size);
		} else {
			printf("failed\n");
		}

		const char *makerStr = exif.ExtractMaker(&size);
		if (makerStr) {
			printf("%s\n", makerStr);
			printf("size: %d\n", size);
		} else {
			printf("failed\n");
		}

		auto makernote = exif.ExtractMakerNote(&size);
		if (makernote) {
			for (u32 i = 0; i < 0x1f; i++)
				printf("%02X", makernote[i]);
			printf("\nsize: 0x%x\n", size);
		} else {
			printf("failed\n");
		}

		auto thumbnail = exif.ExtractThumbnail(&size);
		if (thumbnail) {
			for (u32 i = 0; i < 0x1f; i++)
				printf("%02X", thumbnail[i]);
			printf("\nsize: 0x%x\n", size);
		} else {
			printf("failed\n");
		}

		ams::image::ExifOrientation orientation;
		bool success = exif.ExtractOrientation(&orientation);
		if (success)
			printf("orientation: %d\n", static_cast<u16>(orientation));
		else
			printf("failed\n");

		free(img);
	} else {
		printf("failed to open %s\n", entries[0].fileId.AsString().c_str());
	}

	/*FileId fileId = {0};
	printf("0x%x generate fileId\n", control::GenerateCurrentAlbumFileId(&fileId, 0x1337, ContentType::Screenshot).GetValue());
	printf("%s\n", fileId.AsString().c_str());

	printf("0x%x register aruid\n", control::RegisterAppletResourceUserId(0x420, 0x1337).GetValue());
	printf("0x%x check appId registered\n", control::CheckApplicationIdRegistered(0x1337).GetValue());
	u64 appId;
	printf("0x%x get appId\n", control::GetApplicationIdFromAruid(&appId, 0x420).GetValue());
	printf("%016lX\n", appId);

	Entry entry = {0x1337, fileId};
	ApplicationEntry appEntry;
	printf("0x%x generate appEntry\n", control::GenerateApplicationAlbumEntry(&appEntry, entry, 0x1337).GetValue());
	for (u8 crtr : appEntry.data)
		printf("%hhX", crtr);
	printf("\n");

	printf("0x%x set shim version\n", control::SetShimLibraryVersion(1, 0x420).GetValue());

	printf("0x%x generate appEntry\n", control::GenerateApplicationAlbumEntry(&appEntry, entry, 0x1337).GetValue());
	for (u8 crtr : appEntry.data)
		printf("%hhX", crtr);
	printf("\n");

	printf("0x%x unregister aruid\n", control::UnregisterAppletResourceUserId(0x420, 0x1337).GetValue());*/

	printf("0x%x unmount nand\n", impl::UnmountAlbum(StorageId::Nand).GetValue());
	printf("0x%x unmount sd\n", impl::UnmountAlbum(StorageId::Sd).GetValue());

	ovl::Exit();
	config::Exit();

	splCryptoExit();
	close(sock);
	socketExit();
	return 0;
}