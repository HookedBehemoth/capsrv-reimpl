#include "capsrv_stream_session.hpp"

#include "../server/capsrv_album_server.hpp"

namespace ams::capsrv::server {

    Result AccessorSession::OpenAlbumMovieReadStream(sf::Out<StreamId> stream, const AlbumFileId &fileId) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ControlSession::OpenAlbumMovieReadStream(sf::Out<StreamId> stream, const AlbumFileId &fileId) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result AccessorApplicationSession::OpenAlbumMovieReadStream(sf::Out<StreamId> stream, const sf::ClientProcessId &client_pid, const ApplicationAlbumFileEntry &entry) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::CloseAlbumMovieReadStream(StreamId stream) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::GetAlbumMovieReadStreamMovieDataSize(sf::Out<u64> size, StreamId stream) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::ReadMovieDataFromAlbumMovieReadStream(sf::OutBuffer buffer, sf::Out<u64> size, StreamId stream, u64 offset) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::GetAlbumMovieReadStreamBrokenReason(StreamId stream) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::GetAlbumMovieReadStreamImageDataSize(sf::Out<u64> size, StreamId stream) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::ReadImageDataFromAlbumMovieReadStream(sf::OutBuffer buffer, sf::Out<u64> size, StreamId stream, u64 offset) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::ReadFileAttributeFromAlbumMovieReadStream(sf::Out<CapsScreenShotAttribute> attrs) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::OpenAlbumMovieWriteStream(sf::Out<StreamId> stream, const AlbumFileId &fileId) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::FinishAlbumMovieWriteStream(StreamId stream) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::CommitAlbumMovieWriteStream(StreamId stream) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::DiscardAlbumMovieWriteStream(StreamId stream) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::DiscardAlbumMovieWriteStreamNoDelete(StreamId stream) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::CommitAlbumMovieWriteStreamEx(sf::Out<CapsAlbumEntry> output, StreamId stream) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::StartAlbumMovieWriteStreamDataSection(StreamId stream) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::EndAlbumMovieWriteStreamDataSection(StreamId stream) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::StartAlbumMovieWriteStreamMetaSection(StreamId stream) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::EndAlbumMovieWriteStreamMetaSection(StreamId stream) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::ReadDataFromAlbumMovieWriteStream(sf::OutBuffer out, StreamId stream, u64 offset) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::WriteDataToAlbumMovieWriteStream(sf::InBuffer data, StreamId stream, u64 offset) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::WriteMetaToAlbumMovieWriteStream(sf::InBuffer meta, StreamId stream, u64 offset, u64 unk0, u64 unk1) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::GetAlbumMovieWriteStreamBrokenReason(StreamId stream) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::GetAlbumMovieWriteStreamDataSize(sf::Out<u64> size, StreamId stream) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

    Result ISession::SetAlbumMovieWriteStreamDataSize(StreamId stream, u64 size) {
        AMS_ABORT("Not implemented");
        return ResultSuccess();
    }

}
