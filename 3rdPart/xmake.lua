---@diagnostic disable: undefined-global

add_rules('mode.debug', 'mode.release')

includes('./Marbas.RHI')
includes('./async_simple')

target('ImGuizmo', function()
  set_kind('static')
  set_languages('c11', 'cxx17')
  add_deps('Marbas.RHI')

  add_includedirs(path.join(os.scriptdir(), 'ImGuizmo'), { public = true })
  add_files(path.join(os.scriptdir(), 'ImGuizmo/*.cpp'))
end)

target('IconFontCppHeaders', function()
  set_kind('headeronly')

  add_includedirs(path.join(os.scriptdir(), 'IconFontCppHeaders/'), { public = true })
end)

target('nameof', function()
  set_kind('headeronly')
  add_includedirs(path.join(os.scriptdir(), 'nameof/include'), { public = true })
end)

target('imGuiZMO.quat', function()
  set_kind('static')
  set_languages('c11', 'cxx17')
  add_deps('Marbas.RHI')

  add_defines('VGIZMO_USES_GLM')
  add_packages('glm')

  add_includedirs(path.join(os.scriptdir(), 'imGuIZMO.quat'), { public = true })
  add_files(path.join(os.scriptdir(), 'imGuIZMO.quat/imGuIZMO.quat/*.cpp'))
end)
