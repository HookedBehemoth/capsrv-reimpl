#include "capsrv_stream_session.hpp"

namespace ams::capsrv {

    Result AccessorSession::OpenAlbumMovieReadStream(sf::Out<u64> stream, const FileId &fileId) {
        return ResultSuccess();
    }

    Result ControlSession::OpenAlbumMovieReadStream(sf::Out<u64> stream, const FileId &fileId) {
        return ResultSuccess();
    }

    Result AccessorApplicationSession::OpenAlbumMovieReadStream(sf::Out<u64> stream, const sf::ClientProcessId &client_pid, const CapsApplicationAlbumFileEntry &entry) {
        return ResultSuccess();
    }

    Result ISession::CloseAlbumMovieReadStream(u64 stream) {
        return ResultSuccess();
    }

    Result ISession::GetAlbumMovieReadStreamMovieDataSize(sf::Out<u64> size, u64 stream) {
        return ResultSuccess();
    }

    Result ISession::ReadMovieDataFromAlbumMovieReadStream(sf::OutBuffer buffer, sf::Out<u64> size, u64 stream, u64 offset) {
        return ResultSuccess();
    }

    Result ISession::GetAlbumMovieReadStreamBrokenReason(u64 stream) {
        return ResultSuccess();
    }

    Result ISession::GetAlbumMovieReadStreamImageDataSize(sf::Out<u64> size, u64 stream) {
        return ResultSuccess();
    }

    Result ISession::ReadImageDataFromAlbumMovieReadStream(sf::OutBuffer buffer, sf::Out<u64> size, u64 stream, u64 offset) {
        return ResultSuccess();
    }

    Result ISession::ReadFileAttributeFromAlbumMovieReadStream(sf::Out<CapsScreenShotAttribute> attrs) {
        return ResultSuccess();
    }

    Result ISession::OpenAlbumMovieWriteStream(sf::Out<u64> stream, const FileId &fileId) {
        return ResultSuccess();
    }

    Result ISession::FinishAlbumMovieWriteStream(u64 stream) {
        return ResultSuccess();
    }

    Result ISession::CommitAlbumMovieWriteStream(u64 stream) {
        return ResultSuccess();
    }

    Result ISession::DiscardAlbumMovieWriteStream(u64 stream) {
        return ResultSuccess();
    }

    Result ISession::DiscardAlbumMovieWriteStreamNoDelete(u64 stream) {
        return ResultSuccess();
    }

    Result ISession::CommitAlbumMovieWriteStreamEx(sf::Out<CapsAlbumCommitOutput> output, u64 stream) {
        return ResultSuccess();
    }

    Result ISession::StartAlbumMovieWriteStreamDataSection(u64 stream) {
        return ResultSuccess();
    }

    Result ISession::EndAlbumMovieWriteStreamDataSection(u64 stream) {
        return ResultSuccess();
    }

    Result ISession::StartAlbumMovieWriteStreamMetaSection(u64 stream) {
        return ResultSuccess();
    }

    Result ISession::EndAlbumMovieWriteStreamMetaSection(u64 stream) {
        return ResultSuccess();
    }

    Result ISession::ReadDataFromAlbumMovieWriteStream(sf::OutBuffer out, u64 stream, u64 offset) {
        return ResultSuccess();
    }

    Result ISession::WriteDataToAlbumMovieWriteStream(sf::InBuffer data, u64 stream, u64 offset) {
        return ResultSuccess();
    }

    Result ISession::WriteMetaToAlbumMovieWriteStream(sf::InBuffer meta, u64 stream, u64 offset, u64 unk0, u64 unk1) {
        return ResultSuccess();
    }

    Result ISession::GetAlbumMovieWriteStreamBrokenReason(u64 stream) {
        return ResultSuccess();
    }

    Result ISession::GetAlbumMovieWriteStreamDataSize(sf::Out<u64> size, u64 stream) {
        return ResultSuccess();
    }

    Result ISession::SetAlbumMovieWriteStreamDataSize(u64 stream, u64 size) {
        return ResultSuccess();
    }

    
}
