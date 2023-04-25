#include "LinuxPolicy.hpp"

#include <glog/logging.h>

namespace Marbas {

std::filesystem::path
LinuxSystem::GetCachePath() {
  if (HasEnv("XDG_CACHE_HOME")) {
    auto xdgConfigHome = std::filesystem::path(GetEnv("XDG_CACHE_HOME"));
    if (xdgConfigHome.is_absolute()) {
      return GetEnv("XDG_CACHE_HOME");
    } else {
      LOG(WARNING) << "`XDG_CACHE_HOME` is a relative path. Ignoring its value and falling back to `$HOME/.cache` or "
                      "`get_config_path()` per the XDG Base Directory specification.";
      return HasEnv("HOME") ? std::filesystem::path(GetEnv("HOME")) / ".cache" : ".";
    }
  } else if (HasEnv("HOME")) {
    return std::filesystem::path(GetEnv("HOME")) / ".cache";
  } else {
    return ".";
  }
}

std::filesystem::path
LinuxSystem::GetConfigPath() {
  if (HasEnv("XDG_CONFIG_HOME")) {
    auto xdgConfigHome = std::filesystem::path(GetEnv("XDG_CONFIG_HOME"));
    if (xdgConfigHome.is_absolute()) {
      return GetEnv("XDG_CONFIG_HOME");
    } else {
      LOG(WARNING)
          << "XDG_CONFIG_HOME` is a relative path. Ignoring its value and falling back to `$HOME/.config` or `.` per "
             "the XDG Base Directory specification.";
      return HasEnv("HOME") ? std::filesystem::path(GetEnv("HOME")) / ".config" : ".";
    }
  } else if (HasEnv("HOME")) {
    return std::filesystem::path(GetEnv("HOME")) / ".config";
  } else {
    return ".";
  }
}

}  // namespace Marbas
