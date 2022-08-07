#pragma once

#include <folly/FBString.h>
#include <folly/FBVector.h>
#include <folly/Format.h>

#include <filesystem>

/**
 * tools
 */

// format marco
#define FORMAT(Str, ...) folly::sformat(Str, __VA_ARGS__);

namespace Marbas {

template <typename T>
// #ifdef DEBUG
// using Vector = std::vector<T>;
// #else
using Vector = folly::fbvector<T>;
// #endif

// #ifdef DEBUG
// using String = std::string;
// #else
using String = folly::fbstring;
// #endif

namespace FileSystem = std::filesystem;
using Path = std::filesystem::path;

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
