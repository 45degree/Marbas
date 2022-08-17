---@diagnostic disable: undefined-global

target("Marbas.Test")
    set_kind("binary")
    set_languages("c11", "cxx20")
    add_deps("Marbas.Tool", "Marbas.Core", "Marbas.RHI", "Marbas.Resource")

    add_includedirs("$(projectdir)/src")

    -- Test file
    add_files("$(projectdir)/src/Test/SceneTest/*.cc");
    add_files("$(projectdir)/src/Test/*.cc");

    if is_mode("debug") then
        add_defines("DEBUG")
    end

    on_load(function ()
      local executedir = path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)")

      if not os.isdir(path.join(executedir, "Test")) then
        os.mkdir(path.join(executedir, "Test"))
      end

      os.cp("$(projectdir)/src/Test/*.txt", path.join(executedir, "Test"))
    end)

    add_packages("gtest", "folly", "toml++", "entt", "glfw", "glm", "glog")
target_end()

target("Marbas.RHITest")
  set_kind("binary")
  set_languages("c11", "cxx20")
  -- add_rules("utils.glsl2spv", {
  --     outputdir = path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)", "Test/")
  -- })

  add_files("$(projectdir)/src/Test/RHITest/*.cc");
  -- add_files("$(projectdir)/src/Test/RHITest/*.glsl");
  add_includedirs("$(projectdir)/src")


  on_load(function()
    local executedir = path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)")

    if not os.isdir(path.join(executedir, "Test")) then
      os.mkdir(path.join(executedir, "Test"))
    end

    os.cp("$(projectdir)/src/Test/RHITest/*.glsl", path.join(executedir, "Test"))
    os.cp("$(projectdir)/src/Test/RHITest/*.jpg", path.join(executedir, "Test"))
    os.cp("$(projectdir)/src/Test/RHITest/*.png", path.join(executedir, "Test"))
  end)

  if is_mode("debug") then
      add_defines("DEBUG")
  end

  add_deps("Marbas.RHI")
  add_packages("glfw", "glm", "glog", "glew", "folly", "stb")

target_end()
