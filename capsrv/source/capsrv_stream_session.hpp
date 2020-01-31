#include <stratosphere.hpp>
#include "capsrv_types.hpp"

namespace ams::capsrv {

    class StreamSession : public sf::IServiceObject {
        protected:
            enum class CommandId {
                OpenAlbumMovieReadStream = 2001,
                CloseAlbumMovieReadStream = 2002,
                GetAlbumMovieReadStreamMovieDataSize = 2003,
                ReadMovieDataFromAlbumMovieReadStream = 2004,
                GetAlbumMovieReadStreamBrokenReason = 2005,
                GetAlbumMovieReadStreamImageDataSize = 2006,
                ReadImageDataFromAlbumMovieReadStream = 2007,
                ReadFileAttributeFromAlbumMovieReadStream = 2008,

                OpenAlbumMovieWriteStream = 2401,
                FinishAlbumMovieWriteStream = 2402,
                CommitAlbumMovieWriteStream = 2403,
                DiscardAlbumMovieWriteStream = 2404,
                DiscardAlbumMovieWriteStreamNoDelete = 2405,
                CommitAlbumMovieWriteStreamEx = 2406,
                StartAlbumMovieWriteStreamDataSection = 2411,
                EndAlbumMovieWriteStreamDataSection = 2412,
                StartAlbumMovieWriteStreamMetaSection = 2413,
                EndAlbumMovieWriteStreamMetaSection = 2414,
                ReadDataFromAlbumMovieWriteStream = 2421,
                WriteDataToAlbumMovieWriteStream = 2422,
                WriteMetaToAlbumMovieWriteStream = 2424,
                GetAlbumMovieWriteStreamBrokenReason = 2431,
                GetAlbumMovieWriteStreamDataSize = 2433,
                SetAlbumMovieWriteStreamDataSize = 2434
            };
        public:
            virtual Result OpenAlbumMovieReadStream(sf::Out<u64> stream, const FileId &fileId) = 0;
            virtual Result CloseAlbumMovieReadStream(const u64 stream) = 0;
            virtual Result GetAlbumMovieReadStreamMovieDataSize(sf::Out<u64> size, const u64 stream) = 0;
            virtual Result ReadMovieDataFromAlbumMovieReadStream(sf::OutBuffer buffer, sf::Out<u64> size, const u64 stream, const u64 offset) = 0;
            virtual Result GetAlbumMovieReadStreamBrokenReason(const u64 stream) = 0;
            virtual Result GetAlbumMovieReadStreamImageDataSize(sf::Out<u64> size, const u64 stream) = 0;
            virtual Result ReadImageDataFromAlbumMovieReadStream(sf::OutBuffer buffer, sf::Out<u64> size, const u64 stream, const u64 offset) = 0;
            virtual Result ReadFileAttributeFromAlbumMovieReadStream(sf::Out<CapsScreenshotAttribute> attrs) = 0;
    };

}
