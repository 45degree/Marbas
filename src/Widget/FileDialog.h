#ifndef MARBAS_WIDGET_FILEDIALOG_H
#define MARBAS_WIDGET_FILEDIALOG_H

#include "Widget/Dialog.h"
#include "Common.h"
#include "RHI/RHI.h"

#include "ImGuiFileDialog.h"

#include <optional>

namespace Marbas {

struct FileDialogCrateInfo {
    String fileDialogName;
    String description;
    String fileFilter;
};

class FileDialog : public Dialog {
public:
    explicit FileDialog(const FileDialogCrateInfo& createInfo);

    ~FileDialog() override = default;

public:
    void Open() override;

    void Draw() override;

    [[nodiscard]] std::optional<Path> GetSelectedFile() const noexcept {
        if(!m_isSeleted) return std::nullopt;

        return m_selectedFile;
    }

    void SelectCallback(std::function<void(const char*, const char*)>&& callback) {
        m_callback = callback;
    }

private:
    std::unique_ptr<ImGuiFileDialog> m_instance;

    Path m_selectedFile;
    bool m_isSeleted = false;
    const FileDialogCrateInfo m_createInfo;

    std::function<void(const char*, const char*)> m_callback;
    std::unordered_map<void*, Texture2D*> m_textures;
};

}  // namespace Marbas

#endif
