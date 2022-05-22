#pragma once

#include <memory>
namespace Marbas {

class ICommand;
class CommandBuffer {
 public:
  virtual void
  Clear() = 0;

  virtual void
  BeginRecordCmd() = 0;

  virtual void
  AddCommand(std::unique_ptr<ICommand>&& command) = 0;

  virtual void
  EndRecordCmd() = 0;

  virtual void
  SubmitCommand() = 0;
};

}  // namespace Marbas
