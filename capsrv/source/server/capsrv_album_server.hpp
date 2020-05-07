#pragma once

#include <stratosphere.hpp>

namespace ams::capsrv::server {

    Result Initialize();
    void Exit();

    void AlbumControlServerThreadFunction(void*);
    void AlbumServerThreadFunction(void*);

}
