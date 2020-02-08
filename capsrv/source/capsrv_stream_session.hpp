#pragma once
#include <stratosphere.hpp>

#include "capsrv_types.hpp"

namespace ams::capsrv {

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
	std::array<u64, 4> readStreams;
	std::array<u64, 2> writeStreams;

  public:
	virtual Result CloseAlbumMovieReadStream(u64 stream);
	virtual Result GetAlbumMovieReadStreamMovieDataSize(sf::Out<u64> size, u64 stream);
	virtual Result ReadMovieDataFromAlbumMovieReadStream(sf::OutBuffer buffer, sf::Out<u64> size, u64 stream, u64 offset);
	virtual Result GetAlbumMovieReadStreamBrokenReason(u64 stream);
	virtual Result GetAlbumMovieReadStreamImageDataSize(sf::Out<u64> size, u64 stream);
	virtual Result ReadImageDataFromAlbumMovieReadStream(sf::OutBuffer buffer, sf::Out<u64> size, u64 stream, u64 offset);
	virtual Result ReadFileAttributeFromAlbumMovieReadStream(sf::Out<CapsScreenShotAttribute> attrs);

	virtual Result OpenAlbumMovieWriteStream(sf::Out<u64> stream, const FileId &fileId);
	virtual Result FinishAlbumMovieWriteStream(u64 stream);
	virtual Result CommitAlbumMovieWriteStream(u64 stream);
	virtual Result DiscardAlbumMovieWriteStream(u64 stream);
	virtual Result DiscardAlbumMovieWriteStreamNoDelete(u64 stream);
	virtual Result CommitAlbumMovieWriteStreamEx(sf::Out<CapsAlbumCommitOutput> output, u64 stream);
	virtual Result StartAlbumMovieWriteStreamDataSection(u64 stream);
	virtual Result EndAlbumMovieWriteStreamDataSection(u64 stream);
	virtual Result StartAlbumMovieWriteStreamMetaSection(u64 stream);
	virtual Result EndAlbumMovieWriteStreamMetaSection(u64 stream);
	virtual Result ReadDataFromAlbumMovieWriteStream(sf::OutBuffer out, u64 stream, u64 offset);
	virtual Result WriteDataToAlbumMovieWriteStream(sf::InBuffer data, u64 stream, u64 offset);
	virtual Result WriteMetaToAlbumMovieWriteStream(sf::InBuffer meta, u64 stream, u64 offset, u64 unk0, u64 unk1);
	virtual Result GetAlbumMovieWriteStreamBrokenReason(u64 stream);
	virtual Result GetAlbumMovieWriteStreamDataSize(sf::Out<u64> size, u64 stream);
	virtual Result SetAlbumMovieWriteStreamDataSize(u64 stream, u64 size);
};

class AccessorSession : public ISession {
  public:
	virtual Result OpenAlbumMovieReadStream(sf::Out<u64> stream, const FileId &fileId);

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
	virtual Result OpenAlbumMovieReadStream(sf::Out<u64> stream, const FileId &fileId);

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
		MAKE_SERVICE_COMMAND_META(CommitAlbumMovieWriteStreamEx, hos::Version_700),
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
	virtual Result OpenAlbumMovieReadStream(sf::Out<u64> stream, const sf::ClientProcessId &client_pid, const CapsApplicationAlbumFileEntry &entry);

  public:
	DEFINE_SERVICE_DISPATCH_TABLE{
		MAKE_SERVICE_COMMAND_META(OpenAlbumMovieReadStream),
		MAKE_SERVICE_COMMAND_META(CloseAlbumMovieReadStream),
		MAKE_SERVICE_COMMAND_META(GetAlbumMovieReadStreamMovieDataSize),
		MAKE_SERVICE_COMMAND_META(ReadMovieDataFromAlbumMovieReadStream),
		MAKE_SERVICE_COMMAND_META(GetAlbumMovieReadStreamBrokenReason),
	};
};

} // namespace ams::capsrv
