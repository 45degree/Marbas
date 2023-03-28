#pragma once

#include <filesystem>

#ifdef _WIN32
#include "WindowsPolicy.hpp"
#elif __linux__
#include "LinuxPolicy.hpp"
#endif

namespace Marbas {

template <typename Policy>
class OperatorSystem {
 public:
  static std::filesystem::path
  GetCachePath() {
    return Policy::GetCachePath();
  }

  static std::filesystem::path
  GetConfigPath() {
    return Policy::GetConfigPath();
  }

  static std::string
  GetEnvVar(const std::string& env) {
    return Policy::GetEnvVar();
  }
};

#ifdef _WIN32
using OPSystem = OperatorSystem<WindowsPolicy>;
#elif __linux__
using OPSystem = OperatorSystem<LinuxSystem>;
#endif

}  // namespace Marbas
