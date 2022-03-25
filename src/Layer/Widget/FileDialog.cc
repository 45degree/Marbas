#include "Layer/Widget/FileDialog.h"

namespace Marbas {

FileDialog::FileDialog(const FileDialogCrateInfo& createInfo) :
    m_createInfo(createInfo),
    Widget(createInfo.fileDialogName)
{
    m_rhiFactory = Application::GetRendererFactory();

    ifd::FileDialog::Instance().CreateTexture =
        [&, this](uint8_t* data, int w, int h, char fmt)->void*
    {

        TextureFormatType format = fmt == 0 ? TextureFormatType::BGRA : TextureFormatType::RGBA;
        auto texture = this->m_rhiFactory->CreateTexutre2D(w, h, format);

        texture->SetData(data, w * h * 3);
        texture->UnBind();

        auto tex = texture->GetTexture();

        this->m_textures.insert({tex, texture});

        return tex;
    };

    ifd::FileDialog::Instance().DeleteTexture = [&, this](void* tex) {
        auto iter = this->m_textures.find(tex);
        if(iter == m_textures.end()) {
            LOG(WARNING) << "can't find texture in fileDialog";
        }
        auto texture = this->m_textures.at(tex);
        this->m_rhiFactory->DestoryTexture2D(texture);
        this->m_textures.erase(iter);
    };
}

void FileDialog::Draw() {
    if(m_isShow) {
        ifd::FileDialog::Instance().Open(m_createInfo.fileDialogName.c_str(),
                                         m_createInfo.description.c_str(),
                                         m_createInfo.fileFilter.c_str());

        m_isSeleted = false;
        if (ifd::FileDialog::Instance().IsDone(m_createInfo.fileDialogName.c_str())) {
            if (ifd::FileDialog::Instance().HasResult()) {
                String res = ifd::FileDialog::Instance().GetResult().u8string();
                m_selectedFile = res;
                m_isSeleted = true;
                printf("OPEN[%s]\n", res.c_str());
            }
            ifd::FileDialog::Instance().Close();
        }
    }
}

}  // namespace Marbas
