#include "Common/Model.hpp"

#include <glog/logging.h>
#include <toml++/toml.h>

#include "Resource/ResourceManager.hpp"
#include "Resource/TextureResource.hpp"

namespace Marbas {

String
Model::Serialized() {
  return "";
}

void
Model::Deserialized(const String& str) {}

Model::Model() {}

Model::~Model() {}

}  // namespace Marbas
