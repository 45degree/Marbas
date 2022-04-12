#ifndef MARBAS_LAYER_WIDGET_FILEDIALOG_H
#define MARBAS_LAYER_WIDGET_FILEDIALOG_H

#include "Layer/Widget/Widget.h"
#include "Common.h"
#include "ImFileDialog.h"
#include "Renderer/Interface/RendererFactory.h"

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
    explicit FileDialog(const FileDialogCrateInfo& createInfo) :
        m_createInfo(createInfo),
        Widget(createInfo.fileDialogName)
    {
        m_rendererFactory = Application::GetRendererFactory();

        // ifd::FileDialog::Instance().CreateTexture = CreateFileDialogTexutre;
        ifd::FileDialog::Instance().CreateTexture = [&, this](uint8_t* data, int w, int h, char fmt)->void* {
            if(this->m_texture == nullptr) {
                TextureFormatType format = fmt == 0 ? TextureFormatType::BGRA : TextureFormatType::RGBA;
                this->m_texture = this->m_rendererFactory->CreateTexutre2D(w, h, format);
            }

            this->m_texture->SetData(data, w * h * 3);
            this->m_texture->UnBind();
            return this->m_texture->GetTexture();
        };

        // ifd::FileDialog::Instance().DeleteTexture = DeleteFileDialogTexutre;
        ifd::FileDialog::Instance().DeleteTexture = [&, this](void* tex) {
            this->m_texture = nullptr;
        };
    }

    ~FileDialog() override = default;

public:
    void Draw() override {
        ifd::FileDialog::Instance().Open(m_createInfo.fileDialogName.c_str(),
                                         m_createInfo.description.c_str(),
                                         m_createInfo.fileFilter.c_str());

        m_isSeleted = false;
        if (ifd::FileDialog::Instance().IsDone(m_createInfo.fileDialogName.c_str())) {
            if (ifd::FileDialog::Instance().HasResult()) {
                String res = ifd::FileDialog::Instance().GetResult().u8string();
                m_selectedFile = res.c_str();
                m_isSeleted = true;
                printf("OPEN[%s]\n", res.c_str());
            }
            ifd::FileDialog::Instance().Close();
        }
    }

    [[nodiscard]] std::optional<Path> GetSelectedFile() const noexcept {
        if(!m_isSeleted) return std::nullopt;

        return m_selectedFile;
    }

    // TOOD(45degree): need to change
    static void* CreateFileDialogTexutre(uint8_t* data, int w, int h, char fmt) {

        GLuint tex;

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        return (void*)tex;
    }

    static void DeleteFileDialogTexutre(void* tex) {
        GLuint texID = (GLuint)(size_t)(tex);
        glDeleteTextures(1, &texID);
    }

private:
    RendererFactory* m_rendererFactory = nullptr;
    Texture2D* m_texture = nullptr;
    Path m_selectedFile;
    bool m_isSeleted = false;
    const FileDialogCrateInfo m_createInfo;
};

}  // namespace Marbas

#endif
