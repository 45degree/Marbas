#pragma once

#include "GPUDataPipeline.hpp"

namespace Marbas {

class GIVoxelData final : public GPUDataPipelineDataBase {
  static DescriptorSetArgument s_GIVoxelArgument;
  static Buffer* s_GIVoxelInfo;

 public:
};

};  // namespace Marbas
