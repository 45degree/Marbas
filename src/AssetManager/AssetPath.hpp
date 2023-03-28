#pragma once

#include <fmt/format.h>

#include <functional>
#include <string>

#include "Common/Common.hpp"

namespace Marbas {

class AssetPath {
  friend std::hash<AssetPath>;
  friend struct fmt::formatter<Marbas::AssetPath>;

 public:
  AssetPath();
  AssetPath(const std::string& path);
  AssetPath(const char* path);
  AssetPath(const std::string_view path);

  AssetPath(const AssetPath& obj);

  AssetPath&
  operator=(const AssetPath& obj);

  template <typename Archive>
  void
  serialize(Archive& ar) {
    ar(m_path);
  };

  Path
  GetAbsolutePath() const;

  String
  Stem() const;

  static bool
  ConvertToAssetPath(const Path& path, AssetPath& assetPath);

  bool
  operator==(const AssetPath& obj) const {
    return m_path == obj.m_path;
  }

  std::string
  to_string() const {
    return m_path;
  }

  static bool
  IsSubpath(const Path& path, const Path& base) {
    const auto mismatch_pair = std::mismatch(path.begin(), path.end(), base.begin(), base.end());
    return mismatch_pair.second == base.end();
  }

  static bool
  IsSubpath(const AssetPath& path, const AssetPath& base) {
    const auto mismatch_pair =
        std::mismatch(path.m_path.begin(), path.m_path.end(), base.m_path.begin(), base.m_path.end());
    return mismatch_pair.second == base.m_path.end();
  }

 private:
  std::string m_path;
};

}  // namespace Marbas

namespace std {
template <>
struct hash<Marbas::AssetPath> {
  size_t
  operator()(const Marbas::AssetPath& obj) const {
    return hash<std::string>()(obj.m_path);
  }
};

}  // namespace std

template <>
struct fmt::formatter<Marbas::AssetPath> {
  constexpr fmt::format_parse_context::iterator
  parse(fmt::format_parse_context& ctx) {
    auto iter = ctx.begin();
    return ++iter;
  }

  fmt::format_context::iterator
  format(Marbas::AssetPath path, fmt::format_context& ctx) {
    return fmt::format_to(ctx.out(), "{}", path.m_path);
  }
};
