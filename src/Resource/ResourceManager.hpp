#pragma once

#include <glog/logging.h>

#include <concepts>
#include <mutex>

#include "Common/Common.hpp"
#include "Resource/MaterialResource.hpp"
#include "Resource/ModelResource.hpp"
#include "Resource/ResourceBase.hpp"
#include "Resource/ShaderResource.hpp"
#include "Resource/TextureResource.hpp"

namespace Marbas {

template <typename T>
requires(std::derived_from<T, ResourceBase>) class IResourceContainer {
  using ElementType = T;

 public:
  template <typename... ArgsT>
  std::shared_ptr<T>
  CreateResource(ArgsT&&... args) {
    auto resource = std::make_shared<T>(args...);
    return resource;
  }

  virtual Uid
  AddResource(const std::shared_ptr<T>& resource) = 0;

  virtual void
  RemoveResource(Uid uid) = 0;

  virtual std::shared_ptr<T>
  GetResource(Uid uid) const = 0;

  virtual uint32_t
  GetResourceCount() const = 0;
};

template <typename T>
class DefaultResourceContainer final : public IResourceContainer<T> {
 public:
  Uid
  AddResource(const std::shared_ptr<T>& resource) override {
    Uid uid;
    while (this->m_resources.find(uid) != this->m_resources.end()) {
      uid = Uid();
    }

    this->m_resources[uid] = resource;
    return uid;
  }

  std::shared_ptr<T>
  GetResource(Uid uid) const override {
    if (this->m_resources.find(uid) != this->m_resources.cend()) {
      return this->m_resources.at(uid);
    }
    return nullptr;
  }

  uint32_t
  GetResourceCount() const override {
    return m_resources.size();
  }

  void
  RemoveResource(Uid uid) override {
    auto iter = m_resources.find(uid);
    if (iter != m_resources.cend()) {
      m_resources.erase(iter);
    }
    return;
  }

 private:
  std::unordered_map<Uid, std::shared_ptr<T>> m_resources;
};

using DefaultShaderResourceContainer = DefaultResourceContainer<ShaderResource>;
using DefaultTexture2DResourceContainer = DefaultResourceContainer<Texture2DResource>;
using DefaultTextureCubeMapResourceContainer = DefaultResourceContainer<TextureCubeMapResource>;
using DefaultModelResourceContainer = DefaultResourceContainer<ModelResource>;
using DefaultMaterialResourceContainer = DefaultResourceContainer<MaterialResource>;

class ResourceManager {
 public:
  ResourceManager();
  ResourceManager(const ResourceManager&) = delete;
  ResourceManager(const ResourceManager&&) = delete;

  ResourceManager&
  operator=(const ResourceManager&) = delete;

  ResourceManager&
  operator=(const ResourceManager&&) = delete;

  ~ResourceManager() = default;

 public:
  void
  SetShaderResourceContainer(
      const std::shared_ptr<IResourceContainer<ShaderResource>>& shaderResourceConteiner) {
    m_shaderResourceContainer = shaderResourceConteiner;
  }

  virtual std::shared_ptr<IResourceContainer<ShaderResource>>
  GetShaderResourceContainer() const {
    return m_shaderResourceContainer;
  }

  void
  SetModelResourceContainer(
      const std::shared_ptr<IResourceContainer<ModelResource>>& modelResourceContainer) {
    m_modelResourceContainer = modelResourceContainer;
  }

  virtual std::shared_ptr<IResourceContainer<ModelResource>>
  GetModelResourceContainer() const {
    return m_modelResourceContainer;
  }

  virtual void
  SetTexture2DResourceContainer(
      const std::shared_ptr<IResourceContainer<Texture2DResource>>& texture2DResourceContainer) {
    m_texture2DResourceContainer = texture2DResourceContainer;
  }

  virtual std::shared_ptr<IResourceContainer<Texture2DResource>>
  GetTexture2DResourceContainer() const {
    return m_texture2DResourceContainer;
  }

  virtual std::shared_ptr<IResourceContainer<MaterialResource>>
  GetMaterialResourceContainer() const {
    return m_materialResourceContainer;
  }

  virtual std::shared_ptr<IResourceContainer<TextureCubeMapResource>>
  GetCubeMapResourceContainer() const {
    return m_textureCubeMapResourceContainer;
  }

  void
  SetMaterialResourceContainer(
      const std::shared_ptr<IResourceContainer<MaterialResource>>& materialResourceContainer) {
    m_materialResourceContainer = materialResourceContainer;
  }

  template <typename T>
  std::shared_ptr<IResourceContainer<T>>
  GetContainer() requires(std::derived_from<T, ResourceBase>) {
    if constexpr (std::is_same_v<T, ShaderResource>) {
      return this->GetShaderResourceContainer();
    } else if constexpr (std::is_same_v<T, Texture2DResource>) {
      return m_texture2DResourceContainer;
    } else if constexpr (std::is_same_v<T, TextureCubeMapResource>) {
      return m_textureCubeMapResourceContainer;
    } else if constexpr (std::is_same_v<T, ModelResource>) {
      return this->GetModelResourceContainer();
    }
    return nullptr;
  }

 private:
  std::shared_ptr<IResourceContainer<ShaderResource>> m_shaderResourceContainer;
  std::shared_ptr<IResourceContainer<Texture2DResource>> m_texture2DResourceContainer;
  std::shared_ptr<IResourceContainer<TextureCubeMapResource>> m_textureCubeMapResourceContainer;
  std::shared_ptr<IResourceContainer<ModelResource>> m_modelResourceContainer;
  std::shared_ptr<IResourceContainer<MaterialResource>> m_materialResourceContainer;
  Uid m_defaultShaderResourceUid;
  Uid m_defaultCubeMapShaderResourceUid;
};

}  // namespace Marbas
