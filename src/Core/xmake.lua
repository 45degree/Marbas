---@diagnostic disable: undefined-global

option('CascateCount', function()
  set_default('3')
  set_category('Render Configuration')
  set_description('Max Cascate Count for directional light')
end)

option('DirectionLightCount', function()
  set_default('32')
  set_category('Render Configuration')
  set_description('Max Cascate Count for directional light')
end)

target('Marbas.Core', function()
  set_kind('static')
  set_languages('c11', 'cxx20')

  if has_config('CascateCount') then
    add_defines('CASCATE_COUNT=$(CascateCount)')
  end

  add_defines('MAX_DIRECTION_LIGHT_COUNT=$(DirectionLightCount)')

  add_rules('glslc2spv', {
    outputdir = path.join('$(buildir)', '$(os)', '$(arch)', '$(mode)', 'Shader'),
  })

  add_includedirs('$(projectdir)/src')
  add_files('$(projectdir)/src/Core/Scene/**.cc')
  add_files('$(projectdir)/src/Core/Renderer/*.cc')
  add_files('$(projectdir)/src/Core/Renderer/RenderGraph/*.cc')
  add_files('$(projectdir)/src/Core/Renderer/Pass/GeometryPass.cc')
  add_files('$(projectdir)/src/Core/Renderer/Pass/AtmospherePass.cc')
  add_files('$(projectdir)/src/Core/Renderer/Pass/DirectionLightShadowMapPass.cc')
  add_files('$(projectdir)/src/Core/Renderer/Pass/ForwardPass/*.cc')
  add_files('$(projectdir)/src/Core/Renderer/Pass/PreComputePass/*.cc')
  add_files('$(projectdir)/src/Core/Renderer/Pass/DirectLightPass.cc')
  add_files('$(projectdir)/src/Core/Renderer/Pass/SSAOPass.cc')
  add_files('$(projectdir)/src/Core/Renderer/GI/VXGI/*.cc')

  --
  -- remove_files('Scene/GPUDataPipeline/**.cc')
  remove_files('$(projectdir)/src/Core/Renderer/Pass/PreComputePass/RenderSceneFromProbe.cc')
  remove_files('$(projectdir)/src/Core/Renderer/GI/VXGI/LightInject.*')

  add_includedirs('$(projectdir)/src/Shader')
  add_files('$(projectdir)/src/Shader/geometry.vert')
  add_files('$(projectdir)/src/Shader/geometry.frag')
  add_files('$(projectdir)/src/Shader/grid.vert')
  add_files('$(projectdir)/src/Shader/grid.frag')
  add_files('$(projectdir)/src/Shader/cubeMap.vert')
  add_files('$(projectdir)/src/Shader/cubeMap.frag')
  add_files('$(projectdir)/src/Shader/hdrImage.frag')
  add_files('$(projectdir)/src/Shader/hdrImage.vert')
  add_files('$(projectdir)/src/Shader/hdrImage.geom')
  add_files('$(projectdir)/src/Shader/RenderSceneFromProbe.vert')
  add_files('$(projectdir)/src/Shader/RenderSceneFromProbe.geom')
  add_files('$(projectdir)/src/Shader/RenderSceneFromProbe.frag')
  add_files('$(projectdir)/src/Shader/directionLightShadowMap.vert')
  add_files('$(projectdir)/src/Shader/directionLightShadowMap.geom')
  add_files('$(projectdir)/src/Shader/directionLightShadowMap.frag')
  add_files('$(projectdir)/src/Shader/atmosphere.frag')
  add_files('$(projectdir)/src/Shader/precompute/transmittanceLUT.vert')
  add_files('$(projectdir)/src/Shader/precompute/transmittanceLUT.frag')
  add_files('$(projectdir)/src/Shader/precompute/multiScatterLUT.frag')
  add_files('$(projectdir)/src/Shader/ScreenSpace.vert')
  add_files('$(projectdir)/src/Shader/directLightPass.frag')
  add_files('$(projectdir)/src/Shader/SSAO.frag')
  add_files('$(projectdir)/src/Shader/voxelization.frag')
  add_files('$(projectdir)/src/Shader/voxelization.vert')
  add_files('$(projectdir)/src/Shader/voxelization.geom')
  add_files('$(projectdir)/src/Shader/vxgi.frag')
  add_files('$(projectdir)/src/Shader/Editor/*.frag')
  add_files('$(projectdir)/src/Shader/Editor/*.geom')
  add_files('$(projectdir)/src/Shader/Editor/*.vert')
  add_files('$(projectdir)/src/Shader/lightInject.comp')

  if is_plat('windows') then
    add_files('./Platform/WindowsPolicy.cc')
  elseif is_plat('linux') then
    add_files('./Platform/LinuxPolicy.cc')
  end

  add_deps('Marbas.RHI', 'Marbas.Common', 'IconFontCppHeaders', 'ImGuizmo', 'nameof', 'async_simple')

  add_packages('glfw', 'glm', 'glog', 'assimp', 'entt', 'fmt', 'cereal', 'shaderc', 'abseil')

  if is_plat('windows') then
    add_defines('NOMINMAX')
  end
end)
