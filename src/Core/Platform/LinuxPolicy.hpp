#pragma once

#include <cstdlib>
#include <filesystem>

namespace Marbas {

struct LinuxSystem {
  static std::filesystem::path
  GetCachePath();

  static std::string
  GetEnv(const std::string& env) {
    auto* var = std::getenv(env.c_str());
    if (var == nullptr) return std::string();
    return std::string(var);
  }

  static bool
  HasEnv(const std::string& env) {
    return std::getenv(env.c_str()) == nullptr;
  }

  static std::filesystem::path
  GetConfigPath();
};

}  // namespace Marbas
