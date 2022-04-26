---@diagnostic disable: undefined-global

add_rules("mode.debug", "mode.release")

includes("3rdPart/")

add_requires("glfw 3.3.6")
add_requires("glew 2.2.0")
add_requires("glm 0.9.9+8")
add_requires("glog v0.5.0")
add_requires("folly 2022.02.14")
add_requires("stb 2021.09.10")
add_requires("assimp v5.2.3")
add_requires("toml++ v3.0.0")
add_requires("libiconv 1.16")
add_requires("gtest 1.11.0")
add_requires("entt v3.9.0")
add_requires("uchardet")

includes("src/RHI")
includes("src/Tool")
includes("src/Test")

target("Marbas")
    set_kind("binary")
    set_languages("c11", "cxx20")
    add_rules("utils.glsl2spv", {
        outputdir = path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)", "shader")
    })

    on_load(function()
        local executedir = path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)")

        os.cp("$(projectdir)/resource", executedir)
        os.cp("$(projectdir)/assert", executedir)

        if os.exists(path.join(executedir, "shader")) then
            os.rm(path.join(executedir, "shader"))
        end

        os.mkdir(path.join(executedir, "shader"))
        os.cp("$(projectdir)/src/Shader/*", path.join(executedir, "shader"))
    end)

    if is_mode("debug") then
        add_defines('DEBUG')
    end

    add_includedirs("$(projectdir)/src")
    add_files("$(projectdir)/src/Core/**.cc")
    add_files("$(projectdir)/src/Event/**.cc")
    add_files("$(projectdir)/src/Widget/**.cc")
    add_files("$(projectdir)/src/Layer/**.cc")
    add_files("$(projectdir)/src/Resource/**.cc")
    add_files("$(projectdir)/src/*.cc")

    add_files("$(projectdir)/src/**.glsl")

    add_deps("Imgui", "ImGuizmo", "ImGuiFileDialog", "Marbas.RHI", "Marbas.Tool")

    add_packages("glfw", "glm", "glog", "folly", "assimp",
                 "toml++", "libiconv", "uchardet", "entt")

target_end()
