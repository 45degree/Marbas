#include "FileDialog.hpp"

#include <glog/logging.h>
#include <nfd.h>

namespace Marbas {

FileDialog::FileDialog(const String& filter) : m_filter(filter) {}

void
FileDialog::Open(const Path& defaultPath) {
  nfdchar_t* outPath = nullptr;
  nfdresult_t result = NFD_OpenDialog(m_filter.c_str(), nullptr, &outPath);
  if (result == NFD_OKAY) {
    m_outPath = Path(outPath);
    free(outPath);
  }
  DLOG_IF(INFO, result == NFD_CANCEL) << "User closed the file dialog.";
  DLOG_IF(ERROR, result == NFD_ERROR) << FORMAT("Open File Dialog Error: {}", NFD_GetError());
}

}  // namespace Marbas
