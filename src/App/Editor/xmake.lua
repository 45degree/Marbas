---@diagnostic disable: undefined-global, undefined-field

target("Marbas.Editor")
  set_kind("binary")
  add_files("$(projectdir)/src/App/Editor/**.cc")
  set_languages("c11", "cxx20")

  on_load(function()
    local executedir = path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)")

    os.cp("$(projectdir)/resource", executedir)
    os.cp("$(projectdir)/assert", executedir)

    if not os.exists(path.join(executedir, "Shader")) then
      os.mkdir(path.join(executedir, "Shader"))
    end

    os.cp("$(projectdir)/src/Shader/*", path.join(executedir, "Shader"))
  end)

  add_deps("Marbas.Core")
  add_packages("glfw", "toml++", "folly", "assimp", "glm", "entt", "glog", "nativefiledialog")
target_end()
