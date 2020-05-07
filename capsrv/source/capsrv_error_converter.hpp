#pragma once

#include <stratosphere.hpp>

namespace ams::capsrv {

    namespace {

        Result ConvertErrorImpl(Result rc) {
            if (rc.GetModule() == 206) {
                /* TODO */
            } else {
                /* TODO */
            }
            return rc;
        }

    }

    class AlbumErrorConverter {
      private:
        bool should_convert;

      public:
        void Initialize() {
            this->should_convert = true;
        }

        void Exit() {}

        void SetMode(bool mode) {
            this->should_convert = mode;
        }

        Result ConvertError(Result rc) {
            if (this->should_convert) {
                return ConvertErrorImpl(rc);
            } else {
                return rc;
            }
        }
    };

}
