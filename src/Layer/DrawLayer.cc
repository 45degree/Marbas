#include "Common.h"
#include "Layer/DrawLayer.h"
#include "imgui.h"

#include "Widget/Image.h"

namespace Marbas {

DrawLayer::DrawLayer(): Layer("DrawLayer") {}

DrawLayer::~DrawLayer() = default;

void DrawLayer::OnAttach() {

    FileDialogCrateInfo info {
        "TextureOpenDialog",
        "Open a texture",
        "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga}",
    };

    auto renderImage = std::make_unique<Image>("renderImage");
    m_fileDialog= std::make_unique<FileDialog>(info);
    // m_fileDialog->SetShow(false);

    AddWidget(std::move(renderImage));
    // AddWidget(std::move(fileDialog));
}

void DrawLayer::OnUpdate() {
    for(auto& widget : widgets) {
        ImGui::Begin(widget->GetWidgetName().c_str());
        widget->Draw();
        ImGui::End();
    }
    DrawMenuBar();
}

void DrawLayer::DrawMenuBar() {

    m_fileDialog->SelectCallback([](const char* filePathName, const char* fileName){
        LOG(INFO) << FORMAT("{}, {}", String(filePathName), String(fileName));
    });

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) {
                m_fileDialog->Open();
            }

            if (ImGui::MenuItem("Save", "Ctrl+S"))   { 
            }

            if (ImGui::MenuItem("Close", "Ctrl+W"))  { }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    m_fileDialog->Draw();
}

}  // namespace Marbas
