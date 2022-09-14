#pragma once

#include <folly/FBString.h>
#include <folly/FBVector.h>
#include <folly/Format.h>

#include <filesystem>

/**
 * tools
 */

// format marco
#define FORMAT(Str, ...) folly::sformat(Str, __VA_ARGS__)
#define ROUND_UP(s, n) (((s) + (n)-1) & (~((n)-1)))
#define PATH_TO_CONST_CHAR(Path) reinterpret_cast<const char*>(Path.c_str())

namespace Marbas {

template <typename T>
#ifndef NDEBUG
using Vector = std::vector<T>;
#else
using Vector = folly::fbvector<T>;
#endif

#ifndef NDEBUG
using String = std::string;
using StringView = std::string_view;
#else
using String = folly::fbstring;
using StringView = folly::StringPiece;
#endif

namespace FileSystem = std::filesystem;
using Path = FileSystem::path;

}  // namespace Marbas

namespace folly {

template <>
class FormatValue<Marbas::Path> {
 public:
  explicit FormatValue(const Marbas::Path& val) : val_(val) {}

  template <class FormatCallback>
  void
  format(FormatArg& arg, FormatCallback& cb) const {
    FormatValue<decltype(val_.c_str())>(val_.c_str()).format(arg, cb);
  }

 private:
  const Marbas::Path& val_;
};

}  // namespace folly
