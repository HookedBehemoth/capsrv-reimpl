#pragma once
#include <stratosphere.hpp>
#include <string>

#include "capsrv_types.hpp"

namespace ams::capsrv::config {

void Initialize();
void Exit();

bool SupportsType(ContentType type);
bool StorageValid(StorageId storage);
inline u64 GetMax(StorageId storage, ContentType type);
const char *GetCustomDirectoryPath();

} // namespace ams::capsrv::config