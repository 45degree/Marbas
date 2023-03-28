#pragma once

#include <fmt/format.h>

#include <filesystem>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/**
 * tools
 */

// format marco
#define FORMAT(Str, ...) fmt::format(Str, __VA_ARGS__)
#define ROUND_UP(s, n) (((s) + (n)-1) & (~((n)-1)))
#define PATH_TO_CONST_CHAR(Path) reinterpret_cast<const char*>(Path.c_str())

namespace Marbas {

template <typename T>
using Vector = std::vector<T>;

using String = std::string;
using StringView = std::string_view;

template <typename K, typename V, typename Hash = std::hash<K>, typename EqualTo = ::std::equal_to<K>>
using HashMap = std::unordered_map<K, V, Hash, EqualTo>;
template <typename T, typename Hash = ::std::hash<T>, typename EqualTo = ::std::equal_to<T>>
using HashSet = std::unordered_set<T, Hash, EqualTo>;

namespace FileSystem = std::filesystem;
using Path = FileSystem::path;

// A compile-time method for checking the existence of a class member
// @see https://general-purpose.io/2017/03/10/checking-the-existence-of-a-cpp-class-member-at-compile-time/

// This code uses "decltype" which, according to http://en.cppreference.com/w/cpp/compiler_support
// should be supported by Clang 2.9+, GCC 4.3+ and MSVC 2010+ (if you have an older compiler, please upgrade :)
// As of "constexpr", if not supported by your compiler, you could try "const"
// or use the value as an inner enum value e.g. enum { value = ... }

// check "test_has_member.cpp" for a usage example

/// Defines a "has_member_member_name" class template
///
/// This template can be used to check if its "T" argument
/// has a data or function member called "member_name"
#define define_has_member(member_name)                                    \
  template <typename T>                                                   \
  class has_member_##member_name {                                        \
    typedef char yes_type;                                                \
    typedef long no_type;                                                 \
    template <typename U>                                                 \
    static yes_type                                                       \
    test(decltype(&U::member_name));                                      \
    template <typename U>                                                 \
    static no_type                                                        \
    test(...);                                                            \
                                                                          \
   public:                                                                \
    static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type); \
  }

/// Shorthand for testing if "class_" has a member called "member_name"
///
/// @note "define_has_member(member_name)" must be used
///       before calling "has_member(class_, member_name)"
#define has_member(class_, member_name) has_member_##member_name<class_>::value

}  // namespace Marbas

template <>
struct fmt::formatter<Marbas::Path> {
  constexpr fmt::format_parse_context::iterator
  parse(fmt::format_parse_context& ctx) {
    auto iter = ctx.begin();
    return ++iter;
  }

  fmt::format_context::iterator
  format(Marbas::Path path, fmt::format_context& ctx) {
    return fmt::format_to(ctx.out(), "{}", path.string());
  }
};

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
#define MARBAS_EXPORT __declspec(dllexport)
#elif defined(__GNUC__) && ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3))
#define MARBAS_EXPORT __attribute__((visibility("default")))
#elif defined(__clang__)
#define MARBAS_EXPORT __attribute__((visibility("default")))
#else
#define MARBAS_EXPORT
#endif

// #define MARBAS_EXPORT

#ifdef __cplusplus
}
#endif
