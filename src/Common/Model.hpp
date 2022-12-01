#pragma once

#include "Common/ISerializable.hpp"
#include "Common/MathCommon.hpp"
#include "Common/Mesh.hpp"

namespace Marbas {

class Model final : public ISerializable {
 private:
  String m_modelName;
  Vector<std::shared_ptr<Mesh>> m_meshes;

 public:
  Model();
  virtual ~Model();

 public:
  String
  GetModelName() const {
    return m_modelName;
  }

  void
  SetModelName(const String& name) {
    m_modelName = name;
  }

  String
  Serialized() override;

  void
  Deserialized(const String& str) override;

  void
  AddMesh(const std::shared_ptr<Mesh>& mesh) {
    m_meshes.push_back(mesh);
  }

  const std::shared_ptr<Mesh>
  GetMesh(const uint32_t index) const {
    return m_meshes[index];
  }

  std::shared_ptr<Mesh>
  GetMesh(const uint32_t index) {
    return m_meshes[index];
  }

  uint32_t
  GetMeshesCount() const noexcept {
    return m_meshes.size();
  }
};

}  // namespace Marbas
