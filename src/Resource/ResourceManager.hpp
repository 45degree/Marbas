#ifndef MARBAS_RESOURCE_RESOURCE_MANAGER_HPP
#define MARBAS_RESOURCE_RESOURCE_MANAGER_HPP

#include <glog/logging.h>

#include <concepts>

#include "Common.hpp"
#include "Resource/MaterialResource.hpp"
#include "Resource/ResourceBase.hpp"
#include "Resource/ShaderResource.hpp"
#include "Resource/TextureResource.hpp"

namespace Marbas {

class ResourceManager {
 public:
  ResourceManager();

  ~ResourceManager() = default;

 public:
  std::shared_ptr<Texture2DResource> AddTexture(const Path& imagePath);

  std::shared_ptr<ShaderResource> AddShader(const ShaderFileInfo& shaderFileInfo);

  std::shared_ptr<MaterialResource> AddMaterial();

  std::shared_ptr<CubeMapResource> AddCubeMap(const CubeMapCreateInfo& createInfo);

  void RemoveResource(const Uid& id);

  template <typename T>
  std::shared_ptr<T> FindResource(const Uid& uid) requires std::derived_from<T, ResourceBase> {
    if (m_resources.find(uid) == m_resources.end()) {
      LOG(WARNING) << FORMAT("can't find resurce {}", uid);
      return nullptr;
    }

    auto& resource = m_resources.at(uid);
    std::shared_ptr<T> res = std::dynamic_pointer_cast<T>(resource);
    LOG_IF(WARNING, res == nullptr)
        << FORMAT(" can't convert resource {} to {}, will return nullptr", uid, typeid(T).name());

    return res;
  }

  [[nodiscard]] std::shared_ptr<ShaderResource> GetDefaultCubeMapShader() const noexcept {
    return m_defaultCubeMapShaderResource;
  }

  [[nodiscard]] std::shared_ptr<ShaderResource> GetDefaultShader() const noexcept {
    return m_defaultShaderResource;
  }

 private:
  std::unordered_map<Uid, std::shared_ptr<ResourceBase>> m_resources;
  std::unordered_map<String, Uid> m_staticResourcePath;

  std::shared_ptr<ShaderResource> m_defaultShaderResource;
  std::shared_ptr<ShaderResource> m_defaultCubeMapShaderResource;
};

}  // namespace Marbas

#endif
