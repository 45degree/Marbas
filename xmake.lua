---@diagnostic disable: undefined-global

add_rules("mode.debug", "mode.release")
add_requires("glfw 3.3.5")
add_requires("glew 2.2.0")
add_requires("glm 0.9.9+8")
add_requires("glog v0.5.0")
add_requires("folly 2022.02.14")
add_requires("stb 2021.09.10")
add_requires("assimp v5.2.3")

target("Imgui")
    set_kind("static")
    set_languages("c11", "cxx11")
    add_includedirs("$(projectdir)/3rdPart/imgui", { public = true })
    add_includedirs("$(projectdir)/3rdPart/", { public = true })
    add_includedirs("$(projectdir)/3rdPart/imgui/backends", {public = true})
    add_files("3rdPart/imgui/*.cpp")
    add_files("3rdPart/imgui/backends/imgui_impl_glfw.cpp")
    add_files("3rdPart/imgui/backends/imgui_impl_opengl3.cpp")

    add_packages("glfw")
target_end()

-- target("ImFileDialog")
    -- set_kind("static")
    -- set_languages("c11", "cxx17")
    -- add_deps("Imgui")
    -- add_packages("stb")
    --
    -- if is_plat("windows") then
    --     add_defines("NOMINMAX")
    -- end
    --
    -- add_includedirs("$(projectdir)/3rdPart/ImFileDialog", { public = true })
    -- add_files("$(projectdir)/3rdPart/ImFileDialog/ImFileDialog.cpp")
-- target_end()

target("ImGuiFileDialog")
    set_kind("static")
    set_languages("c11", "cxx17")
    add_deps("Imgui")

    add_files("$(projectdir)/3rdPart/ImGuiFileDialog/**.cpp")
    add_includedirs("$(projectdir)/3rdPart/ImGuiFileDialog", { public = true })
target_end();

target("ImGuizmo")
    set_kind("static")
    set_languages("c11", "cxx11")
    add_includedirs("$(projectdir)/3rdPart/ImGuizmo", { public = true })
    add_deps("Imgui")
    add_files("3rdPart/ImGuizmo/*.cpp")
target_end()

target("Marbas")
    set_kind("binary")
    set_languages("c11", "cxx17")
    add_rules("utils.glsl2spv", {
        outputdir = path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)", "shader")
    })

    on_load(function()
        local executedir = path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)")
        os.cp("$(projectdir)/resource", executedir)
        if os.exists(path.join(executedir, "shader")) then
            os.rm(path.join(executedir, "shader"));
        end
        os.mkdir(path.join(executedir, "shader"))
        os.cp("$(projectdir)/src/Shader/*", path.join(executedir, "shader"))
    end)

    if is_mode("debug") then
        add_defines('DEBUG')
    end

    add_includedirs("$(projectdir)/src")
    add_files("src/**.cc")
    add_files("src/**.vert", "src/**.frag")

    add_deps("Imgui", "ImGuizmo", "ImGuiFileDialog")
    add_packages("glfw", "glm", "glog", "glew", "folly", "stb", "assimp")
target_end()
