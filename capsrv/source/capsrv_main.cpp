#include "capsrv_config.hpp"
#include "capsrv_crypto.hpp"
#include "image/exif_extractor.hpp"
#include "impl/capsrv_controller.hpp"
#include "impl/capsrv_fs.hpp"
#include "impl/capsrv_overlay.hpp"

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

}

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

#define RUNN(function) {\
    u32 rc = function;\
    if (R_FAILED(rc)) printf("0x%x %s\n", function, #function);\
}

using namespace ams::capsrv;

#define RUN(function) {\
    ams::Result rc = function;\
    if (R_FAILED(rc)) printf("0x%x %s expected success\n", rc.GetValue(), #function);\
}

#define FAIL(function) {\
    ams::Result rc = function;\
    if (R_SUCCEEDED(rc)) printf("0x%x %s expected failure\n", rc.GetValue(), #function);\
}

#define TEST(function, var, expected) {\
    RUN(function)\
    if (var != expected) printf("FAILED %s: %s != %s\n", #function, #var, #expected);\
}

int main(int argc, char **argv) {
    socketInitializeDefault();
    int sock = nxlinkStdio();
    setsysInitialize();

    config::Initialize();
    //config::print();

    RUNN(splCryptoInitialize());
    RUN(crypto::Initialize());

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

    /*FILE *f = fopen(entries[1].fileId.GetFilePath().c_str(), "rb");
    if (f) {
        printf("open succ\n");
        fseek(f, 0, SEEK_END);
        auto fsize = ftell(f);
        printf("size: %ld\n", fsize);
        fseek(f, 0, SEEK_SET);
        u8 *img = (u8 *)malloc(fsize);
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
    }*/

    FileId fileId = {0};
    /* Can't test exact equlity since datetime is... time. */
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

    ovl::Exit();
    config::Exit();

    printf("\n\nfinished\n\n\n");

    setsysExit();
    splCryptoExit();
    close(sock);
    socketExit();
    return 0;
}