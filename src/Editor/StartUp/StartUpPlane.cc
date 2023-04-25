#include "StartUpPlane.hpp"

#include <IconsFontAwesome6.h>
#include <imgui/imgui.h>
#include <nfd.h>

#include <iostream>

namespace Marbas {

void
StartUpPlane::Render() {
  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImColor(16, 16, 16).Value);
  ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(29, 29, 29).Value);

  static int Tab = 0;

  // Left side

  ImGui::BeginChild("##left side", ImVec2(230, ImGui::GetContentRegionAvail().y));
  {
    ImVec4 active = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
    ImVec4 inactive = ImGui::GetStyle().Colors[ImGuiCol_Button];

    auto DrawSelectedRect = []() {
      ImVec2 vMin = ImGui::GetItemRectMin();
      ImVec2 vMax = ImGui::GetItemRectMax();

      ImDrawList* draw_list = ImGui::GetWindowDrawList();
      auto y = ImGui::GetItemRectSize().y;
      auto windowPos = ImGui::GetWindowPos();
      // ImGui::Pos
      draw_list->AddRectFilled(vMin, ImVec2(vMin.x + 5, vMax.y), ImColor(255, 0, 0));
    };

    if (ImGui::Button(ICON_FA_CODE "   create a new project", ImVec2(230, 41))) {
      // NFD_OpenDialog()
      nfdchar_t* outputPath = nullptr;
      auto res = NFD_PickFolder(nullptr, &outputPath);
      if (res == NFD_OKAY) {
        m_projectPath = Path(outputPath);
      }
    }

    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Button, Tab == 1 ? active : inactive);
    if (ImGui::Button(ICON_FA_DOWNLOAD "   open a project", ImVec2(230, 41))) Tab = 1;
    if (Tab == 1) DrawSelectedRect();

    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Button, Tab == 2 ? active : inactive);
    if (ImGui::Button(ICON_FA_CODE "   settings", ImVec2(230, 41))) Tab = 2;
    if (Tab == 2) DrawSelectedRect();

    ImGui::PopStyleColor(2);
  }
  ImGui::EndChild();

  ImGui::SameLine();

  ImGui::BeginChild("##right side");
  if (Tab == 1) {
  }
  ImGui::EndChild();

  ImGui::PopStyleColor(2);
}

}  // namespace Marbas
