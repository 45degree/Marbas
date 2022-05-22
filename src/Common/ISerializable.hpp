#pragma once

#include "Common/Common.hpp"

namespace Marbas {

class ISerializable {
 public:
  virtual String
  Serialized() = 0;

  virtual void
  Deserialized(const String& str) = 0;
};

}  // namespace Marbas
