// #pragma once
//
// #include <optional>
//
// #include "Common/MathCommon.hpp"
// #include "RHI/RHI.hpp"
// #include "Resource/ResourceManager.hpp"
// #include "Tool/Uid.hpp"
//
// namespace Marbas {
//
// struct BillBoardComponent_Impl {
//   std::shared_ptr<VertexBuffer> vertexBuffer;
//   std::shared_ptr<DescriptorSet> descriptorSet;
//   std::shared_ptr<Texture2DResource> textureResource;
// };
//
// enum class BillBoardType {
//   POINT_ROT_EYE,
//   ASIX_ROT,
// };
//
// class BillBoardComponent {
//  public:
//   BillBoardType type = BillBoardType::POINT_ROT_EYE;
//   glm::vec3 axis = glm::vec3(0.f, 1.0f, 0.f);
//   glm::vec3 point = glm::vec3(0, 0, 0);
//   glm::vec2 size = glm::vec2(1, 1);
//   std::optional<Uid> textureResourceId;
//   std::shared_ptr<BillBoardComponent_Impl> implData = nullptr;
// };
//
// }  // namespace Marbas
