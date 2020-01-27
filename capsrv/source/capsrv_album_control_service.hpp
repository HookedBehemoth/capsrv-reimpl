#pragma once
#include <stratosphere.hpp>

namespace ams::capsrv {

    /* Service definition. */
    class AlbumControlService final : public sf::IServiceObject {
        protected:
            /* Command IDs. */
            enum class CommandId {
                NotifyAlbumStorageIsAvailable   = 2001,
            };
        public:
            virtual Result NotifyAlbumStorageIsAvailable(const u8 storage);
        public:
            DEFINE_SERVICE_DISPATCH_TABLE {
                MAKE_SERVICE_COMMAND_META(NotifyAlbumStorageIsAvailable),
            };
    };
}