#ifndef MARBAS_LAYER_WIDGET_FILEDIALOG_H
#define MARBAS_LAYER_WIDGET_FILEDIALOG_H

#include "Layer/Widget/Widget.h"
#include "Common.h"
#include "ImFileDialog.h"
#include "RHI/RHI.h"

#include "Core/Application.h"

#include <optional>

namespace Marbas {

struct FileDialogCrateInfo {
    String fileDialogName;
    String description;
    String fileFilter;
};

class FileDialog : public Widget {
public:
    explicit FileDialog(const FileDialogCrateInfo& createInfo);

    ~FileDialog() override = default;

public:
    void Draw() override;

    void Show() {
        m_isShow = true;
    }

    [[nodiscard]] std::optional<Path> GetSelectedFile() const noexcept {
        if(!m_isSeleted) return std::nullopt;

        return m_selectedFile;
    }

private:
    RHIFactory* m_rhiFactory = nullptr;
    Texture2D* m_texture = nullptr;
    Path m_selectedFile;
    bool m_isSeleted = false;
    const FileDialogCrateInfo m_createInfo;

    std::unordered_map<void*, Texture2D*> m_textures;

    bool m_isShow = false;
};

}  // namespace Marbas

#endif
