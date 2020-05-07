#pragma once
#include <stratosphere.hpp>

#include "../capsrv_types.hpp"
#include "../capsrv_stream.hpp"

namespace ams::capsrv::server {

    class ISession : public sf::IServiceObject {
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

      private:
        static inline std::array<StreamId, 4> readStreams;
        static inline std::array<StreamId, 2> writeStreams;

      public:
        virtual Result CloseAlbumMovieReadStream(StreamId stream);
        virtual Result GetAlbumMovieReadStreamMovieDataSize(sf::Out<u64> size, StreamId stream);
        virtual Result ReadMovieDataFromAlbumMovieReadStream(sf::OutBuffer buffer, sf::Out<u64> size, StreamId stream, u64 offset);
        virtual Result GetAlbumMovieReadStreamBrokenReason(StreamId stream);
        virtual Result GetAlbumMovieReadStreamImageDataSize(sf::Out<u64> size, StreamId stream);
        virtual Result ReadImageDataFromAlbumMovieReadStream(sf::OutBuffer buffer, sf::Out<u64> size, StreamId stream, u64 offset);
        virtual Result ReadFileAttributeFromAlbumMovieReadStream(sf::Out<CapsScreenShotAttribute> attrs);

        virtual Result OpenAlbumMovieWriteStream(sf::Out<StreamId> stream, const AlbumFileId &fileId);
        virtual Result FinishAlbumMovieWriteStream(StreamId stream);
        virtual Result CommitAlbumMovieWriteStream(StreamId stream);
        virtual Result DiscardAlbumMovieWriteStream(StreamId stream);
        virtual Result DiscardAlbumMovieWriteStreamNoDelete(StreamId stream);
        virtual Result CommitAlbumMovieWriteStreamEx(sf::Out<CapsAlbumEntry> output, StreamId stream);
        virtual Result StartAlbumMovieWriteStreamDataSection(StreamId stream);
        virtual Result EndAlbumMovieWriteStreamDataSection(StreamId stream);
        virtual Result StartAlbumMovieWriteStreamMetaSection(StreamId stream);
        virtual Result EndAlbumMovieWriteStreamMetaSection(StreamId stream);
        virtual Result ReadDataFromAlbumMovieWriteStream(sf::OutBuffer out, StreamId stream, u64 offset);
        virtual Result WriteDataToAlbumMovieWriteStream(sf::InBuffer data, StreamId stream, u64 offset);
        virtual Result WriteMetaToAlbumMovieWriteStream(sf::InBuffer meta, StreamId stream, u64 offset, u64 unk0, u64 unk1);
        virtual Result GetAlbumMovieWriteStreamBrokenReason(StreamId stream);
        virtual Result GetAlbumMovieWriteStreamDataSize(sf::Out<u64> size, StreamId stream);
        virtual Result SetAlbumMovieWriteStreamDataSize(StreamId stream, u64 size);
    };

    class AccessorSession : public ISession {
      public:
        virtual Result OpenAlbumMovieReadStream(sf::Out<StreamId> stream, const AlbumFileId &fileId);

      public:
        DEFINE_SERVICE_DISPATCH_TABLE{
            MAKE_SERVICE_COMMAND_META(OpenAlbumMovieReadStream),
            MAKE_SERVICE_COMMAND_META(CloseAlbumMovieReadStream),
            MAKE_SERVICE_COMMAND_META(GetAlbumMovieReadStreamMovieDataSize),
            MAKE_SERVICE_COMMAND_META(ReadMovieDataFromAlbumMovieReadStream),
            MAKE_SERVICE_COMMAND_META(GetAlbumMovieReadStreamBrokenReason),
            MAKE_SERVICE_COMMAND_META(GetAlbumMovieReadStreamImageDataSize),
            MAKE_SERVICE_COMMAND_META(ReadImageDataFromAlbumMovieReadStream),
            MAKE_SERVICE_COMMAND_META(ReadFileAttributeFromAlbumMovieReadStream),
        };
    };

    class ControlSession : public ISession {
      public:
        virtual Result OpenAlbumMovieReadStream(sf::Out<StreamId> stream, const AlbumFileId &fileId);

      public:
        DEFINE_SERVICE_DISPATCH_TABLE{
            MAKE_SERVICE_COMMAND_META(OpenAlbumMovieReadStream),
            MAKE_SERVICE_COMMAND_META(CloseAlbumMovieReadStream),
            MAKE_SERVICE_COMMAND_META(GetAlbumMovieReadStreamMovieDataSize),
            MAKE_SERVICE_COMMAND_META(ReadMovieDataFromAlbumMovieReadStream),
            MAKE_SERVICE_COMMAND_META(GetAlbumMovieReadStreamBrokenReason),
            MAKE_SERVICE_COMMAND_META(GetAlbumMovieReadStreamImageDataSize),
            MAKE_SERVICE_COMMAND_META(ReadImageDataFromAlbumMovieReadStream),
            MAKE_SERVICE_COMMAND_META(ReadFileAttributeFromAlbumMovieReadStream),
            MAKE_SERVICE_COMMAND_META(OpenAlbumMovieWriteStream),
            MAKE_SERVICE_COMMAND_META(FinishAlbumMovieWriteStream),
            MAKE_SERVICE_COMMAND_META(CommitAlbumMovieWriteStream),
            MAKE_SERVICE_COMMAND_META(DiscardAlbumMovieWriteStream),
            MAKE_SERVICE_COMMAND_META(DiscardAlbumMovieWriteStreamNoDelete),
            MAKE_SERVICE_COMMAND_META(CommitAlbumMovieWriteStreamEx, hos::Version_7_0_0),
            MAKE_SERVICE_COMMAND_META(StartAlbumMovieWriteStreamDataSection),
            MAKE_SERVICE_COMMAND_META(EndAlbumMovieWriteStreamDataSection),
            MAKE_SERVICE_COMMAND_META(StartAlbumMovieWriteStreamMetaSection),
            MAKE_SERVICE_COMMAND_META(EndAlbumMovieWriteStreamMetaSection),
            MAKE_SERVICE_COMMAND_META(ReadDataFromAlbumMovieWriteStream),
            MAKE_SERVICE_COMMAND_META(WriteDataToAlbumMovieWriteStream),
            MAKE_SERVICE_COMMAND_META(WriteMetaToAlbumMovieWriteStream),
            MAKE_SERVICE_COMMAND_META(GetAlbumMovieWriteStreamBrokenReason),
            MAKE_SERVICE_COMMAND_META(GetAlbumMovieWriteStreamDataSize),
            MAKE_SERVICE_COMMAND_META(SetAlbumMovieWriteStreamDataSize),
        };
    };

    class AccessorApplicationSession : public ISession {
      public:
        virtual Result OpenAlbumMovieReadStream(sf::Out<StreamId> stream, const sf::ClientProcessId &client_pid, const ApplicationAlbumFileEntry &entry);

      public:
        DEFINE_SERVICE_DISPATCH_TABLE{
            MAKE_SERVICE_COMMAND_META(OpenAlbumMovieReadStream),
            MAKE_SERVICE_COMMAND_META(CloseAlbumMovieReadStream),
            MAKE_SERVICE_COMMAND_META(GetAlbumMovieReadStreamMovieDataSize),
            MAKE_SERVICE_COMMAND_META(ReadMovieDataFromAlbumMovieReadStream),
            MAKE_SERVICE_COMMAND_META(GetAlbumMovieReadStreamBrokenReason),
        };
    };

}
