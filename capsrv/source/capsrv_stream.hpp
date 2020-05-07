#pragma once

#include <stratosphere.hpp>

namespace ams::capsrv {

    struct StreamId {
        u64 c;

        ALWAYS_INLINE constexpr bool operator==(const StreamId &id) {
            return this->c == id.c;
        }

        ALWAYS_INLINE constexpr bool operator!=(const StreamId &id) {
            return this->c != id.c;
        }
    };

    class StreamIdGenerator {
      private:
        std::atomic<u64> counter;

      public:
        void Initialize() {
            this->counter = 1;
        }
        void Exit() {
            this->counter = 1;
        }

        StreamId GenerateStreamId() {
            u64 value = this->counter;
            this->counter = value + 1;
            return StreamId{value};
        }

        static StreamId InvalidStreamId() {
            return StreamId{0};
        }
    };

}
