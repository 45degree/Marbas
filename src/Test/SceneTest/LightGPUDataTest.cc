#include <gtest/gtest.h>

#include <algorithm>

#include "Core/Scene/GPUDataPipeline/LightGPUData.hpp"
#include "FakeClass/FakeRHIFactory.hpp"

namespace Marbas::Test {

class LightGPUDataTest : public ::testing::Test {
 protected:
  FakeRHIFactory m_fakeRHIFactory;
};

TEST_F(LightGPUDataTest, LoadLightTest) {
  using async_simple::coro::syncAwait;

  LightGPUData data;
  data.SetRHI(&m_fakeRHIFactory);

  auto& directionLightInfos = LightGPUData::GetDirectionLightInfoList();

  DirectionLightComponent component1, component2;
  syncAwait(data.Load(component1));
  ASSERT_TRUE(component1.lightIndex.has_value());
  ASSERT_EQ(*component1.lightIndex, 0);
  ASSERT_EQ(directionLightInfos.unshadowLightCount, 1);
  ASSERT_EQ(directionLightInfos.shadowLightCount, 0);

  DirectionShadowComponent shadowComponent;
  syncAwait(data.Load(component2, shadowComponent));
  ASSERT_TRUE(component2.lightIndex.has_value());
  ASSERT_EQ(*component2.lightIndex, 1);
  ASSERT_EQ(directionLightInfos.unshadowLightCount, 1);
  ASSERT_EQ(directionLightInfos.shadowLightCount, 1);

  syncAwait(data.Uninstall(component2));
  syncAwait(data.Uninstall(component1));
}

TEST_F(LightGPUDataTest, UpdateLightTest) {
  using async_simple::coro::syncAwait;

  LightGPUData data;
  data.SetRHI(&m_fakeRHIFactory);

  auto& directionLightInfos = LightGPUData::GetDirectionLightInfoList();

  DirectionLightComponent component;
  syncAwait(data.Load(component));
  ASSERT_TRUE(component.lightIndex.has_value());
  ASSERT_EQ(*component.lightIndex, 0);
  ASSERT_EQ(directionLightInfos.unshadowLightCount, 1);
  ASSERT_EQ(directionLightInfos.shadowLightCount, 0);

  // update don't change the index of light, but it will change the list of shadow or unshadow light list;
  DirectionShadowComponent shadow;
  syncAwait(data.Update(component, shadow));
  ASSERT_TRUE(component.lightIndex.has_value());
  ASSERT_EQ(*component.lightIndex, 0);
  ASSERT_EQ(directionLightInfos.unshadowLightCount, 0);
  ASSERT_EQ(directionLightInfos.shadowLightCount, 1);

  syncAwait(data.Uninstall(component));
}

TEST_F(LightGPUDataTest, UninstallDirectionLight) {
  using async_simple::coro::syncAwait;

  LightGPUData data;
  data.SetRHI(&m_fakeRHIFactory);

  auto& directionLightInfos = LightGPUData::GetDirectionLightInfoList();

  DirectionLightComponent component;
  syncAwait(data.Load(component));
  ASSERT_TRUE(component.lightIndex.has_value());
  ASSERT_EQ(*component.lightIndex, 0);
  ASSERT_EQ(directionLightInfos.unshadowLightCount, 1);
  ASSERT_EQ(directionLightInfos.shadowLightCount, 0);

  syncAwait(data.Uninstall(component));
  ASSERT_FALSE(component.lightIndex.has_value());
  ASSERT_EQ(directionLightInfos.unshadowLightCount, 0);
  ASSERT_EQ(directionLightInfos.shadowLightCount, 0);
}

}  // namespace Marbas::Test
