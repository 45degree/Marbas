#include "Resource/ShaderResource.hpp"

#include <glog/logging.h>

#include "Resource/ResourceManager.hpp"
#include "Tool/Util.hpp"

namespace Marbas {

void
ShaderResource::LoadResource(RHIFactory* rhiFactory, const ResourceManager* resourceManager) {
  if (m_isLoad) return;

  m_shader = rhiFactory->CreateShader();

  Path absoultCachePath = resourceManager->GetCachePath();
  if (!FileSystem::exists(absoultCachePath)) {
    FileSystem::create_directories(absoultCachePath);
  }

  for (const auto& [shaderType, shaderStagePath] : m_shaderStagePath) {
    auto shader = rhiFactory->CreateShaderStage(shaderType);
    if (shader->IsEnableSpriv()) {
      // TODO: check if the file is modified
      bool isModefied = false;
      bool isExisted = false;

      // read the file and calculate the sha256
      auto filename = String(shaderStagePath.filename().string()) + ".spv";
      auto absoultCacheFilename = absoultCachePath / filename.c_str();
      isExisted = FileSystem::is_regular_file(absoultCacheFilename);

      if (!isExisted || isModefied) {
        // compile the file
        auto content = shader->CompileFromSource(shaderStagePath);
        WriteBinaryToFile(absoultCacheFilename, content);
      }

      shader->ReadSPIR_V(absoultCacheFilename, "main");
    } else {
      shader->ReadFromSource(shaderStagePath);
    }

    m_shader->AddShaderStage(std::move(shader));
  }

  m_shader->Link();
  m_isLoad = true;
}

}  // namespace Marbas
