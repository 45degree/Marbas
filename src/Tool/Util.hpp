#pragma once

#include <folly/FileUtil.h>

#include <Common/Common.hpp>

namespace Marbas {

String
Sha256(const String& content);

String
ReadFileSource(const Path& path);

String
ReadFileFromBinary(const Path& path);

void
WriteBinaryToFile(const Path& path, const Vector<uint32_t>& name);

}  // namespace Marbas
