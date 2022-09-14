#include "App/Editor/Widget/Information/ModelInformation.hpp"

#include "App/Editor/Widget/FileDialog.hpp"
#include "Core/Scene/Component/ModelComponent.hpp"
#include "Core/Scene/Entity/Entity.hpp"

// clang-format off
#include "imgui.h"
#include "ImGuizmo.h"
// clang-format on

namespace Marbas {

static void
ShowMaterial(Mesh* mesh, ResourceManager* resourceManager) {
  auto container = resourceManager->GetMaterialResourceContainer();
  if (!mesh->m_materialId.has_value()) return;
  auto materialRes = container->GetResource(*(mesh->m_materialId));
  auto ambientTexture = materialRes->GetAmbientOcclusionTexture();
  auto diffuseTexture = materialRes->GetAlbedoTexture();
  auto normalTexture = materialRes->GetNormalTexture();
  auto roughtnessTexture = materialRes->GetRoughnessTexture();
  auto metallicTexture = materialRes->GetMetallicTexture();

  auto regionSize = ImGui::GetContentRegionAvail();

  ImGui::Text("diffuse Texture:");
  if (ImGui::Button("change texture")) {
    FileDialog fileDialog("jpg,png");
    fileDialog.Open(".");
    if (fileDialog.GetResult().has_value()) {
      auto imagePath = fileDialog.GetResult().value();
      auto textureContainer = resourceManager->GetTexture2DResourceContainer();
      auto textureResource = textureContainer->CreateResource(imagePath);
      textureContainer->AddResource(textureResource);
      materialRes->SetAlbedoTexture(textureResource);
      mesh->m_needLoad = true;
    }
  }

  if (diffuseTexture != nullptr) {
    ImGui::Image(diffuseTexture->GetOriginHandle(), ImVec2(regionSize.x, regionSize.x));
  } else {
    ImGui::Text("don't have diffuse Texture");
  }

  ImGui::Separator();

  if (ImGui::Button("change AO texture")) {
    FileDialog fileDialog("jpg,png");
    fileDialog.Open(".");
    if (fileDialog.GetResult().has_value()) {
      auto imagePath = fileDialog.GetResult().value();
      auto textureContainer = resourceManager->GetTexture2DResourceContainer();
      auto textureResource = textureContainer->CreateResource(imagePath);
      textureContainer->AddResource(textureResource);
      materialRes->SetAmbientOcclusionTexture(textureResource);
      mesh->m_needLoad = true;
    }
  }
  ImGui::Text("Ambient Occlusion Texture:");
  if (ambientTexture != nullptr) {
    ImGui::Image(ambientTexture->GetOriginHandle(), ImVec2(regionSize.x, regionSize.x));
  } else {
    ImGui::Text("don't have Ambient Occlusion Texture");
  }

  ImGui::Separator();
  if (ImGui::Button("change normal texture")) {
    FileDialog fileDialog("jpg,png");
    fileDialog.Open(".");
    if (fileDialog.GetResult().has_value()) {
      auto imagePath = fileDialog.GetResult().value();
      auto textureContainer = resourceManager->GetTexture2DResourceContainer();
      auto textureResource = textureContainer->CreateResource(imagePath);
      textureContainer->AddResource(textureResource);
      materialRes->SetNormalTexture(textureResource);
      mesh->m_needLoad = true;
    }
  }
  ImGui::Text("Normal Texture:");
  if (normalTexture != nullptr) {
    ImGui::Image(normalTexture->GetOriginHandle(), ImVec2(regionSize.x, regionSize.x));
  } else {
    ImGui::Text("don't have normal Texture");
  }

  ImGui::Separator();

  if (ImGui::Button("change roughness texture")) {
    FileDialog fileDialog("jpg,png");
    fileDialog.Open(".");
    if (fileDialog.GetResult().has_value()) {
      auto imagePath = fileDialog.GetResult().value();
      auto textureContainer = resourceManager->GetTexture2DResourceContainer();
      auto textureResource = textureContainer->CreateResource(imagePath);
      textureContainer->AddResource(textureResource);
      materialRes->SetRoughnessTexture(textureResource);
      mesh->m_needLoad = true;
    }
  }
  ImGui::Text("Roughness Texture:");
  if (roughtnessTexture != nullptr) {
    ImGui::Image(roughtnessTexture->GetOriginHandle(), ImVec2(regionSize.x, regionSize.x));
  } else {
    ImGui::Text("don't have normal Texture");
  }

  ImGui::Separator();

  if (ImGui::Button("change metallic texture")) {
    FileDialog fileDialog("jpg,png");
    fileDialog.Open(".");
    if (fileDialog.GetResult().has_value()) {
      auto imagePath = fileDialog.GetResult().value();
      auto textureContainer = resourceManager->GetTexture2DResourceContainer();
      auto textureResource = textureContainer->CreateResource(imagePath);
      textureContainer->AddResource(textureResource);
      materialRes->SetMetallicTexture(textureResource);
      mesh->m_needLoad = true;
    }
  }
  ImGui::Text("Metallic Texture:");
  if (metallicTexture != nullptr) {
    ImGui::Image(metallicTexture->GetOriginHandle(), ImVec2(regionSize.x, regionSize.x));
  } else {
    ImGui::Text("don't have metallic Texture");
  }
}

void
ModelInformation::DrawInformation(entt::entity entity, Scene* scene,
                                  ResourceManager* resourceManager) {
  if (!Entity::HasComponent<ModelComponent>(scene, entity)) return;

  auto& modelComponent = Entity::GetComponent<ModelComponent>(scene, entity);
  auto modelResourceContainer = resourceManager->GetModelResourceContainer();
  auto modelResource = modelResourceContainer->GetResource(modelComponent.modelResourceId);
  auto model = modelResource->GetModel();

  ImGui::Text("%s", FORMAT("model name: {}", model->GetModelName()).c_str());
  ImGui::Separator();

  // transform information
  auto modelMatrix = model->GetModelMatrix();

  ImGui::Text("model transform infomation");

  float matrixTranslation[3], matrixRotation[3], matrixScale[3];
  ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix), matrixTranslation,
                                        matrixRotation, matrixScale);

  ImGui::InputFloat3("translate", matrixTranslation);
  ImGui::InputFloat3("rotation", matrixRotation);
  ImGui::InputFloat3("scale", matrixScale);

  ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale,
                                          glm::value_ptr(modelMatrix));

  model->SetModelMatrix(modelMatrix);

  // mesh info
  ImGui::Separator();
  ImGui::Text("mesh infomation");

  auto meshCount = model->GetMeshesCount();
  static int selectMesh = 0;
  Vector<const char*> meshNames;
  for (int i = 0; i < meshCount; i++) {
    auto mesh = model->GetMesh(i);
    meshNames.push_back(mesh->m_name.c_str());
  }
  ImGui::Combo("##meshNames", &selectMesh, meshNames.data(), meshNames.size());

  ImGui::Separator();

  auto mesh = model->GetMesh(selectMesh);
  ImGui::Text("mesh name is :%s", mesh->m_name.c_str());
  ShowMaterial(mesh.get(), resourceManager);
}

}  // namespace Marbas
