---@diagnostic disable: undefined-global

target("Marbas.Resource")
  set_kind("static")
  set_languages("c11", "cxx20")

  add_files("$(projectdir)/src/Resource/**.cc")
  add_includedirs("$(projectdir)/src")

  if is_mode("debug") then
    add_defines('DEBUG')
  end

  add_deps("Marbas.RHI", "Marbas.Tool", "Marbas.Common")
  add_packages("folly", "glog", "toml++", "glfw", "glm", "assimp")
target_end()
