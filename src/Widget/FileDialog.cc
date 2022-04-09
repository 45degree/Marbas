#include "Widget/FileDialog.hpp"
#include "Core/Application.hpp"

#include <ImGuiFileDialog.h>
#include <ImGuiFileDialogConfig.h>

namespace Marbas {

FileDialog::FileDialog(const FileDialogCrateInfo& createInfo) :
    Dialog(createInfo.fileDialogName),
    m_createInfo(createInfo),
    m_instance(std::make_unique<ImGuiFileDialog>())
{
}

void FileDialog::Open() {
    String title = FORMAT("{}: {}", m_createInfo.fileDialogName, m_createInfo.description);
    m_instance->OpenDialog( m_createInfo.fileDialogName.c_str(), title.c_str(),
                            m_createInfo.fileFilter.c_str(), ".");
}

void FileDialog::Draw() {

    if(!m_instance->Display(m_createInfo.fileDialogName.c_str(), ImGuiWindowFlags_NoDocking)) {
        return;
    }

    // action if OK
    if (m_instance->IsOk()) {
        m_callback(m_instance->GetFilePathName().c_str(),
                   m_instance->GetCurrentFileName().c_str());
    }

    // close
    m_instance->Close();
}

}  // namespace Marbas
