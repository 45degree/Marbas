#pragma once

#include <array>
#include <cereal/types/array.hpp>

#include "AssetManager/AssetPath.hpp"
#include "Common/Common.hpp"

namespace Marbas {

struct ImageSky final {
  AssetPath hdrImagePath;

  template <typename Archive>
  void
  serialize(Archive&& ar) {
    ar(hdrImagePath);
  }
};

struct ClearValueSky final {
  std::array<float, 4> clearValue = {0, 0, 0, 1.0};

  template <typename Archive>
  void
  serialize(Archive&& ar) {
    ar(clearValue);
  }
};

struct PhysicalSky final {
  // Rayleigh Scattering Scalar Height
  float rayleighScalarHeight = 8500.f;

  // Mie Scattering Scalar Height
  float mieScalarHeight = 1200.f;

  // Mie Anisotropy (between -1 to 1)
  float mieAnisotropy = 0.5;
  float atmosphereHeight = 60000.f;
  float planetRadius = 6360000.f;
  float ozoneCenterHeight = 25000.f;
  float ozoneWidth = 15000.f;

  template <typename Archive>
  void
  serialize(Archive&& ar) {
    ar(rayleighScalarHeight, mieScalarHeight, mieAnisotropy, atmosphereHeight, planetRadius, ozoneCenterHeight,
       ozoneWidth);
  }
};

struct EnvironmentComponent {
  constexpr static int clearValueItem = 0;
  constexpr static int imageSkyItem = 1;
  constexpr static int physicalSkyItem = 2;

  int currentItem = 0;
  ClearValueSky clearValueSky;
  ImageSky imageSky;
  PhysicalSky physicalSky;

  template <typename Archive>
  void
  serialize(Archive&& ar) {
    ar(currentItem, clearValueSky, imageSky, physicalSky);
  }
};

}  // namespace Marbas
