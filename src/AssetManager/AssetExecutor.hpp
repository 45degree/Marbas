#pragma once

#include <future>

#include "async_simple/Executor.h"

namespace Marbas::Asset::Details {

class AssetExecutor final : public async_simple::Executor {
 public:
  bool
  schedule(Func func) override {
    std::async(std::move(func));
    return true;
  }

  bool
  currentThreadInExecutor() const override {
    throw std::logic_error("not implemented!");
  }

  async_simple::ExecutorStat
  stat() const override {
    throw std::logic_error("not implemented!");
  }

  async_simple::IOExecutor*
  getIOExecutor() override {
    throw std::logic_error("not implemented!");
  }
};

}  // namespace Marbas::Asset::Details
