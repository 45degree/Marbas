#pragma once

#include <cstdlib>
#include <filesystem>

namespace Marbas {

struct WindowsPolicy final {
  static std::filesystem::path
  GetCachePath();

  static std::string
  GetEnv(const std::string& env) {
    char* var;
    size_t len;
    errno_t err = _dupenv_s(&var, &len, env.c_str());
    if (err || var == nullptr) {
      return std::string();
    }
    return std::string(var, len);
  }

  static bool
  HasEnv(const std::string& env) {
    return std::getenv(env.c_str()) == nullptr;
  }

  static std::filesystem::path
  GetConfigPath();
};

}  // namespace Marbas
