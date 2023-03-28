#pragma once

#include <fmt/format.h>

#include <cstdint>
#include <functional>
#include <random>

namespace Marbas {
class Uid;
};

namespace std {
std::string
to_string(Marbas::Uid uid);
}  // namespace std

namespace Marbas {

class Uid {
  friend struct std::hash<Marbas::Uid>;
  friend struct fmt::formatter<Marbas::Uid>;
  friend std::string
  std::to_string(Marbas::Uid uid);

 public:
  Uid();
  ~Uid() = default;

  Uid(const Uid& uid) = default;
  Uid&
  operator=(const Uid& uid) = default;

  bool
  operator==(const Uid& uid) const {
    return m_uid == uid.m_uid;
  }

  template <class Archive>
  void
  serialize(Archive& ar) {
    ar(m_uid);
  }

 private:
  uint64_t m_uid;

 private:
  static std::random_device s_randomDevice;
  static std::mt19937_64 s_gen64;
};

}  // namespace Marbas

namespace std {
template <>
struct hash<Marbas::Uid> {
  size_t
  operator()(const Marbas::Uid& uid) const {
    return hash<uint64_t>()(uid.m_uid);
  }
};

}  // namespace std

template <>
struct fmt::formatter<Marbas::Uid> {
  constexpr fmt::format_parse_context::iterator
  parse(fmt::format_parse_context& ctx) {
    auto iter = ctx.begin();
    return ++iter;
  }

  fmt::format_context::iterator
  format(Marbas::Uid uid, fmt::format_context& ctx) {
    return fmt::format_to(ctx.out(), "{}", uid.m_uid);
  }
};
