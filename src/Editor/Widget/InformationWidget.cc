#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "InformationWidget.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include "AssetManager/GPUAssetUpLoader.hpp"
#include "AssetManager/ModelAsset.hpp"
#include "Common/MathCommon.hpp"
#include "CommonName.hpp"
#include "Core/Scene/Component/EnvironmentComponent.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "Core/Scene/System/RenderSystem.hpp"
#include "DirectionArrowControl.hpp"
#include "ImGuizmo.h"

namespace Marbas {

static std::string
_labelPrefix(const char* const label) {
  float width = ImGui::CalcItemWidth();

  float x = ImGui::GetCursorPosX();
  ImGui::Text("%s", label);
  ImGui::SameLine();
  ImGui::SetCursorPosX(x + width * 0.5f + ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::SetNextItemWidth(-1);

  std::string labelID = "##";
  labelID += label;

  return labelID;
}

template <typename Value, typename DrawFunc>
static void
ShowTextureOrColor(std::optional<AssetPath>& texPath, bool& isUseTex, Value&& color, DrawFunc func) {
  auto texMgr = AssetManager<TextureAsset>::GetInstance();
  auto texGPUMgr = GPUAssetManager<TextureGPUAsset>::GetInstance();

  std::optional<std::shared_ptr<TextureGPUAsset>> gpuAsset;
  if (texPath.has_value()) {
    if (*texPath != "res://" && texMgr->Existed(*texPath)) {
      auto uid = texMgr->Get(*texPath)->GetUid();
      if (texGPUMgr->Exists(uid)) {
        gpuAsset = texGPUMgr->Get(uid);
      }
    }
  }

  ImVec2 imageSize = ImVec2(100, 100);
  ImGui::Text("texture:");
  ImGui::SameLine();
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::CalcItemWidth() * 0.5f + ImGui::GetStyle().ItemInnerSpacing.x);
  if (gpuAsset.has_value()) {
    auto id = (*gpuAsset)->GetImGuiTextureId();
    ImGui::Image(id, imageSize);
  } else {
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor(48, 49, 52).Value);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(48, 49, 52).Value);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor(48, 49, 52).Value);
    ImGui::ButtonEx("##empty image button", imageSize, ImGuiButtonFlags_NoHoveredOnFocus);
    ImGui::PopStyleColor(3);
  }
  if (ImGui::BeginDragDropTarget()) {
    if (auto* payload = ImGui::AcceptDragDropPayload(CONTENT_BROWSER_DRAGDROG); payload != nullptr) {
      const auto& path = *reinterpret_cast<AssetPath*>(payload->Data);
      texPath = path;
    }
    ImGui::EndDragDropTarget();
  }

  ImGui::SameLine();
  ImGui::BeginDisabled(!gpuAsset.has_value());
  ImGui::Checkbox("use", &isUseTex);
  ImGui::EndDisabled();

  func(std::forward<Value>(color));
}

static void
ShowTexture(std::optional<AssetPath>& texPath) {
  auto texMgr = AssetManager<TextureAsset>::GetInstance();
  auto texGPUMgr = GPUAssetManager<TextureGPUAsset>::GetInstance();
  auto regionSize = ImGui::GetContentRegionAvail();

  std::optional<std::shared_ptr<TextureGPUAsset>> gpuAsset;
  if (texPath.has_value()) {
    if (*texPath != "res://" && texMgr->Existed(*texPath)) {
      auto uid = texMgr->Get(*texPath)->GetUid();
      if (texGPUMgr->Exists(uid)) {
        gpuAsset = texGPUMgr->Get(uid);
      }
    }
  }

  if (gpuAsset.has_value()) {
    auto id = (*gpuAsset)->GetImGuiTextureId();
  }
}

static void
ShowMaterial(Mesh& mesh) {
  auto& material = mesh.m_material;

  ImGui::Text("diffuse Texture:");
  ShowTextureOrColor(material.m_diffuseTexturePath, material.m_useDiffuseTexture, material.m_diffuseColor,
                     [](std::array<float, 4>& color) {
                       ImGui::ColorEdit4(_labelPrefix("color").c_str(), color.data(), ImGuiColorEditFlags_AlphaBar);
                     });

  ImGui::Separator();

  ImGui::Text("Ambient Occlusion Texture:");
  ShowTextureOrColor(material.m_ambientTexturePath, material.m_useAmbientTexture, material.m_ambientColor,
                     [](std::array<float, 4>& color) {
                       ImGui::ColorEdit4(_labelPrefix("color").c_str(), color.data(), ImGuiColorEditFlags_AlphaBar);
                     });

  ImGui::Separator();

  ImGui::Text("Normal Texture:");
  ShowTexture(material.m_normalTexturePath);

  ImGui::Separator();

  ImGui::Text("Roughness Texture:");
  ShowTextureOrColor(material.m_roughnessTexturePath, material.m_useRoughnessTexture, material.m_roughnessValue,
                     [](float& value) { ImGui::SliderFloat(_labelPrefix("roughtness").c_str(), &value, 0.f, 1.0f); });

  ImGui::Separator();

  ImGui::Text("Metallic Texture:");
  ShowTextureOrColor(material.m_metalnessTexturePath, material.m_useMetalnessTexture, material.m_metalnessValue,
                     [](float& value) { ImGui::SliderFloat(_labelPrefix("metallic").c_str(), &value, 0.f, 1.0f); });
}

template <typename T, typename DrawComponentFunc>
static void
DrawComponentProp(const std::string_view name, entt::registry& world, entt::entity entity, DrawComponentFunc func) {
  const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                           ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
                                           ImGuiTreeNodeFlags_FramePadding;
  if (world.any_of<T>(entity)) {
    auto& component = world.get<T>(entity);
    ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
    float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImGui::Separator();
    bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, "%s", name.data());
    ImGui::PopStyleVar();

    if (open) {
      func(component);
      ImGui::TreePop();
    }
  }
}

void
InformationWidget::Draw() {
  auto& world = m_scene->GetWorld();
  if (m_entity == entt::null) return;
  DrawComponentProp<ModelSceneNode>("model", world, m_entity, [&](ModelSceneNode& node) {
    std::string modelpath = node.modelPath.to_string();

    ImGui::InputText(_labelPrefix("model path:").c_str(), modelpath.data(), modelpath.size(),
                     ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget()) {
      if (auto* payload = ImGui::AcceptDragDropPayload(CONTENT_BROWSER_DRAGDROG); payload != nullptr) {
        const auto& path = *reinterpret_cast<AssetPath*>(payload->Data);
        // TODO: check path
        node.modelPath = path;
        modelpath = path.to_string();
      }
      ImGui::EndDragDropTarget();
    }

    // mesh info
    ImGui::Separator();
    ImGui::Text("mesh infomation");

    if (modelpath == "res://") return;

    auto modelAssetMgr = AssetManager<ModelAsset>::GetInstance();
    if (!modelAssetMgr->Existed(modelpath)) return;

    auto modelAsset = modelAssetMgr->Get(modelpath);

    auto meshCount = modelAsset->GetMeshCount();
    static int selectMesh = 0;
    Vector<const char*> meshNames;
    for (int i = 0; i < meshCount; i++) {
      const auto& mesh = modelAsset->GetMesh(i);
      meshNames.push_back(mesh.m_name.c_str());
    }
    ImGui::Combo("##meshNames", &selectMesh, meshNames.data(), meshNames.size());

    ImGui::Separator();

    auto& mesh = modelAsset->GetMesh(selectMesh);
    ImGui::Text("mesh name is :%s", mesh.m_name.c_str());
    ShowMaterial(mesh);
  });

  DrawComponentProp<EnvironmentComponent>("environment", world, m_entity, [&](EnvironmentComponent& node) {
    std::array<const char*, 3> items = {"clear value", "image", "physical sky"};

    ImGui::Combo(_labelPrefix("environment type").c_str(), &node.currentItem, items.data(), items.size());

    if (node.currentItem == 0) {
      auto* color = node.clearValueSky.clearValue.data();
      ImGui::ColorEdit4(_labelPrefix("clear color").c_str(), color, ImGuiColorEditFlags_AlphaPreviewHalf);
    } else if (node.currentItem == 1) {
      auto path = node.imageSky.hdrImagePath.to_string();
      ImGui::InputText(_labelPrefix("image res: ").c_str(), path.data(), path.size(), ImGuiInputTextFlags_ReadOnly);
      if (ImGui::BeginDragDropTarget()) {
        if (auto* payload = ImGui::AcceptDragDropPayload(CONTENT_BROWSER_DRAGDROG); payload != nullptr) {
          const auto& path = *reinterpret_cast<AssetPath*>(payload->Data);
          // TODO: check path
          if (path != "res://") {
            node.imageSky.hdrImagePath = path;
            // RenderSystem::RerunPreComputePass("hdrImagePass", m_rhiFactory);
          }
        }

        ImGui::EndDragDropTarget();
      }
    } else if (node.currentItem == EnvironmentComponent::physicalSkyItem) {
      auto& sky = node.physicalSky;
      ImGui::InputFloat(_labelPrefix("Atmosphere Height").c_str(), &sky.atmosphereHeight);
      ImGui::InputFloat(_labelPrefix("Rayleigh Scatter Scalar Height").c_str(), &sky.rayleighScalarHeight);
      ImGui::InputFloat(_labelPrefix("Mie Scatter Scalar Height").c_str(), &sky.mieScalarHeight);
      ImGui::SliderFloat(_labelPrefix("Mie Anisotropy").c_str(), &sky.mieAnisotropy, -1, 1);
      ImGui::InputFloat(_labelPrefix("Ozone Center Height").c_str(), &sky.ozoneCenterHeight);
      ImGui::InputFloat(_labelPrefix("Ozone Width").c_str(), &sky.ozoneWidth);
      if (ImGui::Button("recompute sky")) {
        RenderSystem::RerunPreComputePass("TransmittanceLUTPass", m_rhiFactory);
      }
    }
  });

  DrawComponentProp<TransformComp>("transform", world, m_entity, [&](TransformComp& transform) {
    float translation[3], rotation[3], scale[3];
    auto modelMatrix = transform.GetGlobalTransform();
    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix), translation, rotation, scale);

    if (world.any_of<ModelSceneNode>(m_entity)) {
      ImGui::InputFloat3(_labelPrefix("translate").c_str(), translation);
      ImGui::InputFloat3(_labelPrefix("rotation").c_str(), rotation);
      ImGui::InputFloat3(_labelPrefix("scale").c_str(), scale);

    } else if (world.any_of<DirectionalLightSceneNode>(m_entity)) {
      ImGui::InputFloat3(_labelPrefix("translate").c_str(), translation);
    }

    ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, glm::value_ptr(modelMatrix));
    transform.SetGlobalTransform(modelMatrix);
  });

  DrawComponentProp<DirectionLightComponent>("directional light", world, m_entity, [&](DirectionLightComponent& node) {
    auto& light = node.m_light;
    auto color = light.GetColor();
    auto dir = light.GetDirection();
    ImGui::InputFloat3("light", glm::value_ptr(color));
    light.SetColor(color);
    ImGui::InputFloat3("directional", glm::value_ptr(dir));
    dir = glm::normalize(dir);
    light.SetDirection(dir);

    auto regionSize = ImGui::GetContentRegionAvail();
    DirectionArrowControl({regionSize.x, regionSize.x}, dir);

    bool isSun = world.any_of<SunLightTag>(m_entity);
    ImGui::Checkbox("Set as Sun", &isSun);

    if (isSun) {
      if (!world.any_of<SunLightTag>(m_entity)) {
        auto sunView = world.view<SunLightTag>();
        for (auto entity : sunView) {
          world.remove<SunLightTag>(entity);
        }
        world.emplace<SunLightTag>(m_entity);
      }
    }
  });

  // if (world.any_of<ModelSceneNode>(m_entity)) {
  //   m_modelInformation.DrawInformation(m_entity, m_scene, m_resourceManager);
  // } else if (world.any_of<DirectionalLightSceneNode>(m_entity)) {
  //   m_lightInformation.DrawInformation(m_entity, m_scene, m_resourceManager);
  // }
}

InformationWidget::InformationWidget(RHIFactory* rhiFactory, Scene* scene)
    : Widget("Infomation", rhiFactory), m_scene(scene) {}

}  // namespace Marbas