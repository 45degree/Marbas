#pragma once

#include <functional>

#include "Common/Common.hpp"
#include "Core/Scene/Scene.hpp"

namespace Marbas {

class Widget {
 public:
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

 public:
  explicit Widget(const String& widgetName, RHIFactory* rhiFactory);
  Widget(const Widget&) = default;
  virtual ~Widget();

 public:
  [[nodiscard]] const String&
  GetWidgetName() const noexcept {
    return m_widgetName;
  }

  virtual void
  Draw() = 0;

 protected:
  const String m_widgetName;
  RHIFactory* m_rhiFactory;
};

}  // namespace Marbas
