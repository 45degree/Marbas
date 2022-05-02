#ifndef MARBAS_RHI_INTERFACE_DRAWBATCH_HPP
#define MARBAS_RHI_INTERFACE_DRAWBATCH_HPP

#include "RHI/Interface/DrawBatch.hpp"

namespace Marbas {

class OpenGLDrawBatch : public DrawBatch {
 public:
  OpenGLDrawBatch() = default;
  ~OpenGLDrawBatch() = default;

 public:
  void Draw() override;
};

}  // namespace Marbas

#endif
