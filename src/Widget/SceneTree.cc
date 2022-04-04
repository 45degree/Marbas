#include "Widget/SceneTree.h"

#include <imgui.h>

namespace Marbas {

void SceneTreeWidget::Draw() {
    if(ImGui::TreeNode("RootScene")){

        const char* names[5] = { "Label1", "Label2", "Label3", "Label4", "Label5" };
        for (int n = 0; n < 5; n++) {
            ImGui::Selectable(names[n]);
            if (ImGui::BeginPopupContextItem())
            {
                ImGui::Text("This a popup for \"%s\"!", names[n]);
                if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Right-click to open popup");
        }

        ImGui::TreePop();
    }
}

}  // namespace Marbas
