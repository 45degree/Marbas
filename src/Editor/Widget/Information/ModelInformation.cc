// #include "ModelInformation.hpp"
//
// #include "AssetManager/ModelAsset.hpp"
// #include "Core/Scene/Component/HierarchyComponent.hpp"
// #include "Editor/Widget/AssetSelectDialog.hpp"
// #include "Editor/Widget/FileDialog.hpp"
//
// // clang-format off
// #include "imgui.h"
// #include "ImGuizmo.h"
// // clang-format on
//
// namespace Marbas {
//
// static void
// ShowMaterial(Mesh* mesh, ResourceManager* resourceManager) {
//   auto container = resourceManager->GetMaterialResourceContainer();
//   if (!mesh->m_materialId.has_value()) return;
//   auto materialRes = container->GetResource(*(mesh->m_materialId));
//   auto ambientTexture = materialRes->GetAmbientOcclusionTexture();
//   auto diffuseTexture = materialRes->GetAlbedoTexture();
//   auto normalTexture = materialRes->GetNormalTexture();
//   auto roughtnessTexture = materialRes->GetRoughnessTexture();
//   auto metallicTexture = materialRes->GetMetallicTexture();
//
//   auto regionSize = ImGui::GetContentRegionAvail();
//   const static std::string imageExtension = "jpg,png,tga";
//   ImGui::Text("diffuse Texture:");
//   if (ImGui::Button("change texture")) {
//     FileDialog fileDialog(imageExtension);
//     fileDialog.Open(".");
//     if (fileDialog.GetResult().has_value()) {
//       auto imagePath = fileDialog.GetResult().value();
//       auto textureContainer = resourceManager->GetTexture2DResourceContainer();
//       auto textureResource = textureContainer->CreateResource(imagePath);
//       textureContainer->AddResource(textureResource);
//       materialRes->SetAlbedoTexture(textureResource);
//     }
//   }
//
//   if (diffuseTexture != nullptr) {
//     ImGui::Image(diffuseTexture->GetImguiTextureId(), ImVec2(regionSize.x, regionSize.x));
//   } else {
//     ImGui::Text("don't have diffuse Texture");
//   }
//
//   ImGui::Separator();
//
//   if (ImGui::Button("change AO texture")) {
//     FileDialog fileDialog(imageExtension);
//     fileDialog.Open(".");
//     if (fileDialog.GetResult().has_value()) {
//       auto imagePath = fileDialog.GetResult().value();
//       auto textureContainer = resourceManager->GetTexture2DResourceContainer();
//       auto textureResource = textureContainer->CreateResource(imagePath);
//       textureContainer->AddResource(textureResource);
//       materialRes->SetAmbientOcclusionTexture(textureResource);
//     }
//   }
//   ImGui::Text("Ambient Occlusion Texture:");
//   if (ambientTexture != nullptr) {
//     ImGui::Image(ambientTexture->GetImguiTextureId(), ImVec2(regionSize.x, regionSize.x));
//   } else {
//     ImGui::Text("don't have Ambient Occlusion Texture");
//   }
//
//   ImGui::Separator();
//   if (ImGui::Button("change normal texture")) {
//     FileDialog fileDialog(imageExtension);
//     fileDialog.Open(".");
//     if (fileDialog.GetResult().has_value()) {
//       auto imagePath = fileDialog.GetResult().value();
//       auto textureContainer = resourceManager->GetTexture2DResourceContainer();
//       auto textureResource = textureContainer->CreateResource(imagePath);
//       textureContainer->AddResource(textureResource);
//       materialRes->SetNormalTexture(textureResource);
//     }
//   }
//   ImGui::Text("Normal Texture:");
//   if (normalTexture != nullptr) {
//     ImGui::Image(normalTexture->GetImguiTextureId(), ImVec2(regionSize.x, regionSize.x));
//   } else {
//     ImGui::Text("don't have normal Texture");
//   }
//
//   ImGui::Separator();
//
//   if (ImGui::Button("change roughness texture")) {
//     FileDialog fileDialog(imageExtension);
//     fileDialog.Open(".");
//     if (fileDialog.GetResult().has_value()) {
//       auto imagePath = fileDialog.GetResult().value();
//       auto textureContainer = resourceManager->GetTexture2DResourceContainer();
//       auto textureResource = textureContainer->CreateResource(imagePath);
//       textureContainer->AddResource(textureResource);
//       materialRes->SetRoughnessTexture(textureResource);
//     }
//   }
//   ImGui::Text("Roughness Texture:");
//   if (roughtnessTexture != nullptr) {
//     ImGui::Image(roughtnessTexture->GetImguiTextureId(), ImVec2(regionSize.x, regionSize.x));
//   } else {
//     ImGui::Text("don't have normal Texture");
//   }
//
//   ImGui::Separator();
//
//   if (ImGui::Button("change metallic texture")) {
//     FileDialog fileDialog(imageExtension);
//     fileDialog.Open(".");
//     if (fileDialog.GetResult().has_value()) {
//       auto imagePath = fileDialog.GetResult().value();
//       auto textureContainer = resourceManager->GetTexture2DResourceContainer();
//       auto textureResource = textureContainer->CreateResource(imagePath);
//       textureContainer->AddResource(textureResource);
//       materialRes->SetMetallicTexture(textureResource);
//     }
//   }
//   ImGui::Text("Metallic Texture:");
//   if (metallicTexture != nullptr) {
//     ImGui::Image(metallicTexture->GetImguiTextureId(), ImVec2(regionSize.x, regionSize.x));
//   } else {
//     ImGui::Text("don't have metallic Texture");
//   }
// }
//
// void
// ModelInformation::DrawInformation(entt::entity entity, Scene* scene, ResourceManager* resourceManager) {
//   auto& world = scene->GetWorld();
//   auto& modelHierarchyComponent = world.get<TransformComp>(entity);
//   auto& modelSceneNode = world.get<ModelSceneNode>(entity);
//
//   // if (!modelSceneNode.modelUid) return;
//
//   auto modelManager = AssetManager<ModelAsset>::GetInstance();
//   // auto model = modelManager->Get(modelSceneNode.modelPath);
//   // auto modelResourceContainer = resourceManager->GetModelResourceContainer();
//   // auto modelResource = modelResourceContainer->GetResource(*modelSceneNode.modelUid);
//   // auto model = modelResource->GetModel();
//
//   ImGui::Text("%s", FORMAT("model name: {}", modelSceneNode.modelName).c_str());
//   ImGui::SameLine();
//   if (ImGui::Button("select")) {
//     ImGui::OpenPopup("AssetSelectDialog");
//   }
//
//   if (ImGui::BeginPopupModal("AssetSelectDialog")) {
//     ImGui::EndPopup();
//   }
//
//   ImGui::Separator();
//
//   // transform information
//   // auto modelMatrix = model->GetModelMatrix();
//   auto modelMatrix = modelHierarchyComponent.GetGlobalTransform();
//
//   ImGui::Text("model transform infomation");
//
//   float matrixTranslation[3], matrixRotation[3], matrixScale[3];
//   ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix), matrixTranslation, matrixRotation, matrixScale);
//
//   ImGui::InputFloat3("translate", matrixTranslation);
//   ImGui::InputFloat3("rotation", matrixRotation);
//   ImGui::InputFloat3("scale", matrixScale);
//
//   ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale,
//   glm::value_ptr(modelMatrix));
//
//   // HierarchyComponent::ResetGlobalTransformMatrix(scene->GetWorld(), entity, modelMatrix);
//   modelHierarchyComponent.SetGlobalTransform(modelMatrix);
//
//   // mesh info
//   ImGui::Separator();
//   ImGui::Text("mesh infomation");
//
//   // auto meshCount = model->GetMeshesCount();
//   // static int selectMesh = 0;
//   // Vector<const char*> meshNames;
//   // for (int i = 0; i < meshCount; i++) {
//   //   auto mesh = model->GetMesh(i);
//   //   meshNames.push_back(mesh->m_name.c_str());
//   // }
//   // ImGui::Combo("##meshNames", &selectMesh, meshNames.data(), meshNames.size());
//   //
//   // ImGui::Separator();
//   //
//   // auto mesh = model->GetMesh(selectMesh);
//   // ImGui::Text("mesh name is :%s", mesh->m_name.c_str());
//   // ShowMaterial(mesh.get(), resourceManager);
// }
//
// }  // namespace Marbas
