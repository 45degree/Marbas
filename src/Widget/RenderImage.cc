#include "Widget/RenderImage.h"
#include "RHI/RHI.h"
#include "Common.h"
#include "Core/Window.h"
#include "Core/Application.h"

namespace Marbas {

void Image::Draw() {
    auto window = Application::GetApplicationsWindow();
    auto m_renderLayer = dynamic_cast<RenderLayer*>(window->GetLayer("RenderLayer"));
    if(m_renderLayer == nullptr) return;

    imageSize = ImGui::GetContentRegionAvail();
    auto camera = m_renderLayer->GetCamera();

    float aspect = imageSize.x / imageSize.y;
    camera->SetAspect(aspect);

    auto textureId = const_cast<ImTextureID>(m_renderLayer->GetFrameBufferTexture());
    ImGui::Image(textureId, imageSize, ImVec2(0, 1), ImVec2(1, 0));
}

}  // namespace Marbas
