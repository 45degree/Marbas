#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#define VGIZMO_USES_GLM

#include "GuiInformationWindow.hpp"

#include <glog/logging.h>
#include <imGuIZMO.quat/imGuIZMOquat.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <concepts>

#include "AssetManager/ModelAsset.hpp"
#include "CommonName.hpp"
#include "Core/Scene/Component/Component.hpp"
#include "Core/Scene/GPUDataPipeline/TextureGPUData.hpp"
#include "Core/Scene/SceneManager.hpp"
#include "Core/Scene/System/RenderSystem.hpp"
#include "ImGuizmo.h"

namespace Marbas::Gui {

// static std::string
// _labelPrefix(const char* const label) {
//   float width = ImGui::CalcItemWidth();
//
//   float x = ImGui::GetCursorPosX();
//   ImGui::Text("%s", label);
//   ImGui::SameLine();
//   ImGui::SetCursorPosX(x + width * 0.5f + ImGui::GetStyle().ItemInnerSpacing.x);
//   ImGui::SetNextItemWidth(-1);
//
//   std::string labelID = "##";
//   labelID += label;
//
//   return labelID;
// }

static bool
ShowTextureOrColor(std::optional<AssetPath>& texPath, bool& isUseTex) {
  auto texMgr = AssetManager<TextureAsset>::GetInstance();
  auto texGPUMgr = TextureGPUDataManager::GetInstance();

  std::optional<std::shared_ptr<TextureGPUData>> gpuAsset;
  if (texPath.has_value()) {
    if (*texPath != "res://" && texMgr->Existed(*texPath)) {
      auto asset = texMgr->Get(*texPath);
      if (texGPUMgr->Existed(*asset)) {
        gpuAsset = texGPUMgr->TryGet(*asset);
      }
    }
  }

  ImVec2 imageSize = ImVec2(100, 100);
  ImGui::Text("texture:");
  ImGui::SameLine();
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

  bool texChanged = false;
  if (ImGui::BeginDragDropTarget()) {
    if (auto* payload = ImGui::AcceptDragDropPayload(CONTENT_BROWSER_DRAGDROG); payload != nullptr) {
      const auto& path = *reinterpret_cast<AssetPath*>(payload->Data);
      texPath = path;
      texChanged = true;
    }
    ImGui::EndDragDropTarget();
  }

  ImGui::SameLine();
  ImGui::BeginDisabled(!gpuAsset.has_value());
  bool isClicked = ImGui::Checkbox("use", &isUseTex);
  ImGui::EndDisabled();

  return texChanged || isClicked;
}

/**
 * @brief Show the texture and color in the widget
 *
 * @tparam Value color value type
 * @tparam DrawFunc show color function
 * @param texPath texture path
 * @param isUseTex use texture or color
 * @param color color
 * @param func show color function
 * @return whether texture has changed
 */
template <typename Value, typename DrawFunc>
static bool
ShowTextureOrColor(std::optional<AssetPath>& texPath, bool& isUseTex, Value&& color, DrawFunc func) {
  auto changed = ShowTextureOrColor(texPath, isUseTex);
  func(std::forward<Value>(color));
  return changed;
}

static bool
ShowMaterial(Mesh& mesh) {
  auto& material = mesh.m_material;
  auto& texChanged = mesh.m_materialTexChanged;
  auto& valueChanged = mesh.m_materialValueChanged;

  auto ShowComponent = [](const std::string& title, auto&& func) {
    const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                             ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
                                             ImGuiTreeNodeFlags_FramePadding;
    if (ImGui::TreeNodeEx(title.c_str(), treeNodeFlags)) {
      func();
      ImGui::TreePop();
    }
  };

  ShowComponent("diffuse Texture", [&]() {
    auto& path = material.m_diffuseTexturePath;
    auto& useTex = material.m_useDiffuseTexture;
    auto& color = material.m_diffuseColor;

    texChanged |= ShowTextureOrColor(path, useTex, color, [&](std::array<float, 4>& color) {
      ImGui::Text("color");
      ImGui::SameLine();
      valueChanged |= ImGui::ColorEdit4("##color", color.data(), ImGuiColorEditFlags_AlphaBar);
    });
  });

  ShowComponent("Normal Texture", [&]() {
    auto& path = material.m_normalTexturePath;
    auto& useTex = material.m_useNormalTexture;
    texChanged |= ShowTextureOrColor(path, useTex);
  });

  ShowComponent("Roughness Texture", [&]() {
    auto& path = material.m_roughnessTexturePath;
    auto& useTex = material.m_useRoughnessTexture;
    auto& value = material.m_roughnessValue;

    texChanged |= ShowTextureOrColor(path, useTex, value, [&](float& value) {
      ImGui::Text("roughtness");
      ImGui::SameLine();
      valueChanged |= ImGui::SliderFloat("##roughtness", &value, 0.f, 1.0f);
    });
  });

  ShowComponent("Metallic Texture", [&]() {
    auto& path = material.m_metalnessTexturePath;
    auto& useTex = material.m_useMetalnessTexture;
    auto& value = material.m_metalnessValue;
    texChanged |= ShowTextureOrColor(path, useTex, value, [&](float& value) {
      ImGui::Text("metallic");
      ImGui::SameLine();
      valueChanged |= ImGui::SliderFloat("##metallic", &value, 0.f, 1.0f);
    });
  });
  return true;
}

template <typename T, ComponentUpdateFunc<T> DrawComponentFunc>
static void
DrawComponentProp(const std::string_view name, Scene* scene, entt::entity entity, DrawComponentFunc&& func) {
  const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                           ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
                                           ImGuiTreeNodeFlags_FramePadding;
  if (scene->AnyOf<T>(entity)) {
    // const auto& component = world.get<T>(entity);
    ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
    float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImGui::Separator();
    bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, "%s", name.data());
    ImGui::PopStyleVar();

    if (open) {
      scene->Update<T>(entity, std::move(func));
      ImGui::TreePop();
    }
  }
}

void
GuiInformationWindow::OnDraw() {
  auto sceneManager = SceneManager::GetInstance();
  auto activeScene = sceneManager->GetActiveScene();
  // auto& world = activeScene->GetWorld();
  if (m_entity == entt::null) return;
  DrawComponentProp<ModelSceneNode>("model", activeScene, m_entity, [&](ModelSceneNode& node) {
    std::string modelpath = node.modelPath.to_string();

    ImGui::Text("model path:");
    ImGui::SameLine();
    ImGui::InputText("##model path:", modelpath.data(), modelpath.size(), ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget()) {
      if (auto* payload = ImGui::AcceptDragDropPayload(CONTENT_BROWSER_DRAGDROG); payload != nullptr) {
        const auto& path = *reinterpret_cast<AssetPath*>(payload->Data);
        // TODO: check path
        node.modelPath = path;
        return true;
        modelpath = path.to_string();
      }
      ImGui::EndDragDropTarget();
    }

    // mesh info
    ImGui::Separator();
    ImGui::Text("mesh infomation");

    if (modelpath == "res://") return false;

    auto modelAssetMgr = AssetManager<ModelAsset>::GetInstance();
    if (!modelAssetMgr->Existed(modelpath)) return false;

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
    return ShowMaterial(mesh);
  });

  DrawComponentProp<EnvironmentComponent>("environment", activeScene, m_entity, [&](EnvironmentComponent& node) {
    std::array<const char*, 3> items = {"clear value", "image", "physical sky"};
    int currentItem = node.currentItem;

    bool changeEnvironment = false;

    ImGui::Text("environment type");
    ImGui::SameLine();
    changeEnvironment |= ImGui::Combo("##environment type", &node.currentItem, items.data(), items.size());

    if (node.currentItem == 0) {
      auto* color = node.clearValueSky.clearValue.data();
      ImGui::Text("clear color");
      ImGui::SameLine();
      changeEnvironment |= ImGui::ColorEdit4("##clear color", color, ImGuiColorEditFlags_AlphaPreviewHalf);
    } else if (node.currentItem == 1) {
      auto path = node.imageSky.hdrImagePath.to_string();
      ImGui::Text("image res:");
      ImGui::SameLine();
      ImGui::InputText("##image res:", path.data(), path.size(), ImGuiInputTextFlags_ReadOnly);
      if (ImGui::BeginDragDropTarget()) {
        if (auto* payload = ImGui::AcceptDragDropPayload(CONTENT_BROWSER_DRAGDROG); payload != nullptr) {
          const auto& path = *reinterpret_cast<AssetPath*>(payload->Data);
          // TODO: check path
          if (path != "res://") {
            node.imageSky.hdrImagePath = path;
            changeEnvironment |= true;
            // RenderSystem::RerunPreComputePass("hdrImagePass", m_rhiFactory);
          }
        }

        ImGui::EndDragDropTarget();
      }
    } else if (node.currentItem == EnvironmentComponent::physicalSkyItem) {
      auto& sky = node.physicalSky;
      ImGui::Text("Atmosphere Height");
      ImGui::SameLine();
      if (ImGui::InputFloat("##Atmosphere Height", &node.physicalSky.atmosphereHeight)) {
        return true;
      }

      ImGui::Text("Rayleigh Scatter Scalar Height");
      ImGui::SameLine();
      changeEnvironment |= ImGui::InputFloat("##Rayleigh Scatter Scalar Height", &sky.rayleighScalarHeight);

      ImGui::Text("Mie Scatter Scalar Height");
      ImGui::SameLine();
      changeEnvironment |= ImGui::InputFloat("##Mie Scatter Scalar Height", &sky.mieScalarHeight);

      ImGui::Text("Mie Anisotropy");
      ImGui::SameLine();
      changeEnvironment |= ImGui::SliderFloat("##Mie Anisotropy", &sky.mieAnisotropy, -1, 1);

      ImGui::Text("Ozone Center Height");
      ImGui::SameLine();
      changeEnvironment |= ImGui::InputFloat("##Ozone Center Height", &sky.ozoneCenterHeight);

      ImGui::Text("Ozone Width");
      ImGui::SameLine();
      changeEnvironment |= ImGui::InputFloat("##Ozone Width", &sky.ozoneWidth);

      if (ImGui::Button("recompute sky")) {
        RenderSystem::RerunPreComputePass("TransmittanceLUTPass", m_rhiFactory);
      }
    }
    return changeEnvironment;
  });

  DrawComponentProp<TransformComp>("transform", activeScene, m_entity, [&](TransformComp& transform) {
    float translation[3], rotation[3], scale[3];
    auto modelMatrix = transform.GetGlobalTransform();
    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix), translation, rotation, scale);

    const auto& world = activeScene->GetWorld();
    if (world.any_of<ModelSceneNode>(m_entity)) {
      ImGui::Text("translate");
      ImGui::SameLine();
      ImGui::InputFloat3("##translate", translation);

      ImGui::Text("rotation");
      ImGui::SameLine();
      ImGui::InputFloat3("##rotation", rotation);

      ImGui::Text("scale");
      ImGui::SameLine();
      ImGui::InputFloat3("##scale", scale);

    } else if (world.any_of<DirectionalLightSceneNode>(m_entity)) {
      ImGui::Text("translate");
      ImGui::SameLine();
      ImGui::InputFloat3("##translate", translation);
    } else if (world.any_of<VXGIProbeSceneNode>(m_entity)) {
      ImGui::Text("translate");
      ImGui::SameLine();
      ImGui::InputFloat3("##translate", translation);
    }

    ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, glm::value_ptr(modelMatrix));
    transform.SetGlobalTransform(modelMatrix);
    return true;
  });

  DrawComponentProp<DirectionLightComponent>("directional light", activeScene, m_entity, [&](auto& node) {
    auto& color = node.m_color;
    auto& energy = node.m_energy;
    auto& dir = node.m_direction;
    const auto& world = activeScene->GetWorld();

    bool changeDirectionLight = false;

    ImGui::SeparatorText("light");
    changeDirectionLight |= ImGui::ColorEdit3("color", glm::value_ptr(color));
    changeDirectionLight |= ImGui::InputFloat("energy", &energy);

    ImGui::SeparatorText("direction");
    changeDirectionLight |= ImGui::InputFloat3("directional", glm::value_ptr(dir));
    changeDirectionLight |= ImGui::gizmo3D("##gizmo1", dir);

    auto regionSize = ImGui::GetContentRegionAvail();

    bool isSun = activeScene->AnyOf<SunLightTag>(m_entity);
    ImGui::Checkbox("Set as Sun", &isSun);

    if (isSun) {
      if (!activeScene->AnyOf<SunLightTag>(m_entity)) {
        auto sunView = world.view<SunLightTag>();
        for (auto entity : sunView) {
          activeScene->Remove<SunLightTag>(m_entity);
        }
        activeScene->Emplace<SunLightTag>(m_entity);
      }
    }
    return changeDirectionLight;
  });

  DrawComponentProp<DirectionShadowComponent>("directional light shadow", activeScene, m_entity, [&](auto& node) {
    auto& split = node.m_split;
    bool changeFlag = false;
    for (int i = 0; i < node.m_split.size(); i++) {
      std::string name = "split" + std::to_string(i);
      changeFlag |= ImGui::SliderFloat(name.c_str(), &split[i], 0, 1);
    }

    return changeFlag;
  });

  DrawComponentProp<VXGIProbeSceneNode>("vxgi probe", activeScene, m_entity, [&](auto& node) {
    bool changeFlag = false;
    ImGui::Text("probe size:");
    ImGui::SameLine();

    auto& size = node.size;
    changeFlag |= ImGui::InputFloat3("##probe size:", glm::value_ptr(size));
    return changeFlag;
  });
}

GuiInformationWindow::GuiInformationWindow(RHIFactory* rhiFactory)
    : GuiWindow("Infomation"), m_rhiFactory(rhiFactory) {}

}  // namespace Marbas::Gui
