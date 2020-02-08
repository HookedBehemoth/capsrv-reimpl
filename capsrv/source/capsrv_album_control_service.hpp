#pragma once
#include <stratosphere.hpp>

#include "capsrv_stream_session.hpp"
#include "capsrv_types.hpp"

namespace ams::capsrv {

/* Service definition. */
class AlbumControlService final : public sf::IServiceObject {
  protected:
	/* Command IDs. */
	enum class CommandId {
		//CaptureRawImage                         = 1,
		//CaptureRawImageWithTimeout              = 2,
		SetShimLibraryVersion = 33,
		//RequestTakingScreenShot                 = 1001,
		//RequestTakingScreenShotWithTimeout      = 1002,
		//NotifyTakingScreenShotRefused           = 1011,
		NotifyAlbumStorageIsAvailable = 2001,
		NotifyAlbumStorageIsUnavailable = 2002,
		RegisterAppletResourceUserId = 2011,
		UnregisterAppletResourceUserId = 2012,
		GetApplicationIdFromAruid = 2013,
		CheckApplicationIdRegistered = 2014,
		GenerateCurrentAlbumFileId = 2101,
		GenerateApplicationAlbumEntry = 2102,
		SaveAlbumScreenShotFile = 2201,
		SaveAlbumScreenShotFileEx = 2202,
		SetOverlayScreenShotThumbnailData = 2301,
		SetOverlayMovieThumbnailData = 2302,
		OpenControlSession = 60001,
	};

  public:
	//virtual Result CaptureRawImage();
	//virtual Result CaptureRawImageWithTimeout();
	virtual Result SetShimLibraryVersion(u64 version, u64 aruid);
	//virtual Result RequestTakingScreenShot();
	//virtual Result RequestTakingScreenShotWithTimeout();
	//virtual Result NotifyTakingScreenShotRefused();
	virtual Result NotifyAlbumStorageIsAvailable(StorageId storage);
	virtual Result NotifyAlbumStorageIsUnavailable(StorageId storage);
	virtual Result RegisterAppletResourceUserId(u64 aruid);
	virtual Result UnregisterAppletResourceUserId(u64 aruid);
	virtual Result GetApplicationIdFromAruid(sf::Out<u64> appId, u64 aruid);
	virtual Result CheckApplicationIdRegistered(u64 appId);
	virtual Result GenerateCurrentAlbumFileId(sf::Out<FileId> out, u64 appId, ContentType type);
	virtual Result GenerateApplicationAlbumEntry(sf::Out<ApplicationEntry> out, u64 appId, ContentType type);
	virtual Result SaveAlbumScreenShotFile(sf::InBuffer buffer, const FileId &fileId);
	virtual Result SaveAlbumScreenShotFileEx(sf::InNonSecureBuffer buffer, const FileId &fileId, u64 unk0, u64 unk1, u64 unk2);
	virtual Result SetOverlayScreenShotThumbnailData(sf::InNonSecureBuffer buffer, const FileId &fileId);
	virtual Result SetOverlayMovieThumbnailData(sf::InNonSecureBuffer buffer, const FileId &fileId);
	virtual Result OpenControlSession(sf::Out<std::shared_ptr<ControlSession>> out, const sf::ClientAppletResourceUserId &aruid);

  public:
	DEFINE_SERVICE_DISPATCH_TABLE{
		//MAKE_SERVICE_COMMAND_META(CaptureRawImage,                    hos::Version_100, hos::Version_100),
		//MAKE_SERVICE_COMMAND_META(CaptureRawImageWithTimeout          hos::Version_100, hos::Version_100),
		MAKE_SERVICE_COMMAND_META(SetShimLibraryVersion),
		//MAKE_SERVICE_COMMAND_META(RequestTakingScreenShot),
		//MAKE_SERVICE_COMMAND_META(RequestTakingScreenShotWithTimeout),
		//MAKE_SERVICE_COMMAND_META(NotifyTakingScreenShotRefused),
		MAKE_SERVICE_COMMAND_META(NotifyAlbumStorageIsAvailable),
		MAKE_SERVICE_COMMAND_META(NotifyAlbumStorageIsUnavailable),
		MAKE_SERVICE_COMMAND_META(RegisterAppletResourceUserId),
		MAKE_SERVICE_COMMAND_META(UnregisterAppletResourceUserId),
		MAKE_SERVICE_COMMAND_META(GetApplicationIdFromAruid),
		MAKE_SERVICE_COMMAND_META(CheckApplicationIdRegistered),
		MAKE_SERVICE_COMMAND_META(GenerateCurrentAlbumFileId),
		MAKE_SERVICE_COMMAND_META(GenerateApplicationAlbumEntry),
		MAKE_SERVICE_COMMAND_META(SaveAlbumScreenShotFile),
		MAKE_SERVICE_COMMAND_META(SaveAlbumScreenShotFileEx),
		MAKE_SERVICE_COMMAND_META(SetOverlayScreenShotThumbnailData),
		MAKE_SERVICE_COMMAND_META(SetOverlayMovieThumbnailData),
		MAKE_SERVICE_COMMAND_META(OpenControlSession),
	};
};
} // namespace ams::capsrv