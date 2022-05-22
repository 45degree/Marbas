// #ifndef MARBARS_RHI_INTERFACE_VIEWPORT_H
// #define MARBARS_RHI_INTERFACE_VIEWPORT_H
//
// #include <tuple>
//
// #include "Common/Common.hpp"
//
// namespace Marbas {
//
// class Viewport {
//  public:
//   Viewport() = default;
//   virtual ~Viewport() = default;
//
//  public:
//   void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
//     this->x = x;
//     this->y = y;
//     this->width = width;
//     this->height = height;
//   }
//
//   virtual void UseViewport() const = 0;
//
//   [[nodiscard]] std::tuple<int, int, int, int> GetViewport() const noexcept {
//     return std::make_tuple(x, y, width, height);
//   }
//
//  protected:
//   uint32_t x, y;
//   uint32_t width, height;
// };
//
// }  // namespace Marbas
//
// #endif
