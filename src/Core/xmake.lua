---@diagnostic disable: undefined-global

target('Marbas.Core', function()
  set_kind('static')
  set_languages('c11', 'cxx20')

  add_rules('glslc2spv', {
    outputdir = path.join('$(buildir)', '$(os)', '$(arch)', '$(mode)', 'Shader'),
  })

  add_includedirs('$(projectdir)/src')
  add_files('$(projectdir)/src/Core/Scene/**.cc')
  add_files('$(projectdir)/src/Core/Renderer/*.cc')
  -- add_files('$(projectdir)/src/Core/Renderer/GI/**.cc')
  add_files('$(projectdir)/src/Core/Renderer/RenderGraph/*.cc')
  add_files('$(projectdir)/src/Core/Renderer/Pass/GeometryPass.cc')
  add_files('$(projectdir)/src/Core/Renderer/Pass/AtmospherePass.cc')
  add_files('$(projectdir)/src/Core/Renderer/Pass/DirectionLightShadowMapPass.cc')
  add_files('$(projectdir)/src/Core/Renderer/Pass/ForwardPass/*.cc')
  add_files('$(projectdir)/src/Core/Renderer/Pass/PreComputePass/*.cc')

  --
  remove_files('$(projectdir)/src/Core/Renderer/Pass/PreComputePass/RenderSceneFromProbe.cc')

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

  if is_plat('windows') then
    add_files('./Platform/WindowsPolicy.cc')
  elseif is_plat('linux') then
    add_files('./Platform/LinuxPolicy.cc')
  end

  add_deps('Marbas.RHI', 'Marbas.Common', 'IconFontCppHeaders', 'ImGuizmo', 'nameof')

  add_packages('glfw', 'glm', 'glog', 'assimp', 'entt', 'fmt', 'cereal')

  if is_plat('windows') then
    add_defines('NOMINMAX')
  end
end)
