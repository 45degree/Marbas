#include "Widget/RenderImage.h"
#include "RHI/RHI.h"
#include "Common.h"
#include "Core/Window.h"
#include "Core/Application.h"

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
