#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "DirectionArrowControl.hpp"

#include <imgui_internal.h>

#include <iostream>

namespace Marbas {

static ImVec2
World2Pos(const ImVec2& size, const ImVec2& position, const glm::vec3& point) {
  static auto viewMatrix = glm::lookAt(glm::vec3(-2, 2, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  static auto projectMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.f);
  static auto mvpMatrix = projectMatrix * viewMatrix;

  auto trans = mvpMatrix * glm::vec4(point, 1.0);
  trans /= trans.w;
  trans.x = (1 + trans.x) / 2.;
  trans.y = (1 + trans.y) / 2.;
  // trans.y = 1.f - trans.y;
  trans.x *= size.x;
  trans.y *= size.y;
  trans.x += position.x;
  trans.y += position.y;
  return ImVec2(trans.x, trans.y);
}

void
DirectionArrowControl(const ImVec2& size, glm::vec3& direction) {
  ImGuiWindow* window = ImGui::GetCurrentWindow();
  if (window->SkipItems) return;

  const ImVec2 currentPos = ImGui::GetCursorScreenPos();
  ImRect bb(currentPos, currentPos + size);
  ImGui::ItemSize(bb);
  if (!ImGui::ItemAdd(bb, 0)) return;

  ImDrawList* drawList = ImGui::GetWindowDrawList();
  if (!drawList) return;

  auto startP = World2Pos(size, currentPos, -direction);
  auto endP = World2Pos(size, currentPos, glm::vec3(0, 0, 0));
  drawList->AddLine(startP, endP, IM_COL32(255, 255, 0, 255), 3.0f);
  // drawList->AddTriangleFilled()
  // drawList->AddTriangleFilled(currentPos, const ImVec2 &p2, const ImVec2 &p3, ImU32 col)
}

}  // namespace Marbas
