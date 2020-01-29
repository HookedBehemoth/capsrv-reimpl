#pragma once

#include <stratosphere/sf.hpp>

namespace ams::capsrv {

    /* Service definition. */
    class AlbumApplicationService final : public sf::IServiceObject {
        protected:
            /* Command IDs. */
            enum class CommandId {
                SetShimLibraryVersion   = 33,
            };
        public:
            virtual Result SetShimLibraryVersion(const u64 version, const u64 aruid);
        public:
            DEFINE_SERVICE_DISPATCH_TABLE {
                MAKE_SERVICE_COMMAND_META(SetShimLibraryVersion),
            };
    };
}