---@diagnostic disable: undefined-global

target('Marbas.Common', function()
  set_kind('shared')
  set_languages('c11', 'cxx20')

  add_includedirs('$(projectdir)/src')
  add_files('$(projectdir)/src/Common/**.cc')
  add_packages('abseil', 'assimp', 'toml++', 'glm', 'fmt', 'cereal')
end)
