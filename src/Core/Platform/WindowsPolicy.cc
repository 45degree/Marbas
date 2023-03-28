#include "WindowsPolicy.hpp"

#include <algorithm>

namespace Marbas {

std::filesystem::path
WindowsPolicy::GetCachePath() {
  if (HasEnv("LOCALAPPDATA")) {
    auto var = GetEnv("LOCALAPPDATA");
    std::replace(var.begin(), var.end(), '/', '\\');
    return var;
  } else if (HasEnv("TEMP")) {
    auto var = GetEnv("TEMP");
    std::replace(var.begin(), var.end(), '/', '\\');
    return var;
  } else
    return GetConfigPath();
}

std::filesystem::path
WindowsPolicy::GetConfigPath() {
  if (HasEnv("APPDATA")) {
    auto var = GetEnv("APPDATA");
    std::replace(var.begin(), var.end(), '/', '\\');
    return var;
  }
  return ".";
}

}  // namespace Marbas
