#pragma once

#include <optional>

#include "Common/Common.hpp"
#include "Dialog.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

class FileDialog {
 public:
  explicit FileDialog(const String& filter);
  ~FileDialog() = default;

 public:
  void
  Open(const Path& defaultPath);

  const std::optional<Path>&
  GetResult() {
    return m_outPath;
  }

 private:
  String m_filter;
  std::optional<Path> m_outPath = std::nullopt;
};

class FloderDialog {};

}  // namespace Marbas
