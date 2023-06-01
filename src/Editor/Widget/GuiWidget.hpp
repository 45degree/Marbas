#pragma once

#include <entt/entt.hpp>
#include <string>

#include "GuiBase.hpp"

namespace Marbas::Gui {

template <typename T>
class Signal;

template <typename R, typename... Args>
class Signal<R(Args...)> {
 public:
  entt::sigh<R(Args...)> m_sign;
  entt::sink<entt::sigh<R(Args...)>> m_sink = {m_sign};

 public:
  template <auto Func, typename Type>
  void
  Connect(Type&& instance) {
    m_sink.template connect<Func>(std::forward<Type>(instance));
  }

  void
  Publish(Args... args) const {
    m_sign.publish(std::forward<Args>(args)...);
  }
};

class GuiWidget : public GuiBase {
 public:
  GuiWidget(const std::string& name) : m_name(name) {}
  ~GuiWidget() override = default;

  void
  Draw() override {
    OnDraw();
  }

 protected:
  virtual void
  OnDraw() = 0;

 protected:
  std::string m_name;
};

};  // namespace Marbas::Gui
