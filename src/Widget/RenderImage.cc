#include "Widget/RenderImage.hpp"
#include "RHI/RHI.hpp"
#include "Common.hpp"
#include "Core/Window.hpp"
#include "Core/Application.hpp"

namespace Marbas {

void Image::Draw() {
    auto window = Application::GetApplicationsWindow();
    auto renderLayer = window->GetRenderLayer();
    if(renderLayer == nullptr) return;

    imageSize = ImGui::GetContentRegionAvail();
    auto camera = renderLayer->GetCamera();

    float aspect = imageSize.x / imageSize.y;
    camera->SetAspect(aspect);

    auto textureId = const_cast<ImTextureID>(renderLayer->GetFrameBufferTexture());
    ImGui::Image(textureId, imageSize, ImVec2(0, 1), ImVec2(1, 0));
}

}  // namespace Marbas
