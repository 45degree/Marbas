#ifndef MARBARS_COMMON_H
#define MARBARS_COMMON_H

#include <folly/FBString.h>
#include <folly/FBVector.h>
#include <folly/Format.h>

#include <filesystem>

/**
 * tools
 */

#define FORMAT(Str, ...) folly::sformat(Str, __VA_ARGS__);

namespace Marbas {

template <typename T>
#ifdef DEBUG
using Vector = std::vector<T>;
#else
using Vector = folly::fbvector<T>;
#endif

#ifdef DEBUG
using String = std::string;
#else
using String = folly::fbstring;
#endif

namespace FileSystem = std::filesystem;
using Path = std::filesystem::path;

}  // namespace Marbas

#endif
