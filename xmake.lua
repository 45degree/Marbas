---@diagnostic disable: undefined-global

add_rules("mode.debug", "mode.release")
add_requires("glfw 3.3.5")
add_requires("glad")
add_requires("glew 2.2.0")
add_requires("glm 0.9.9+8")
add_requires("glog v0.5.0")
add_requires("folly")

target("Marbas")
    set_kind("binary")
    set_languages("c11", "cxx20")
    add_rules("utils.glsl2spv", {
        outputdir = path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)", "shader")
    })

    if is_mode("debug") then
        add_defines('DEBUG')
    end

    add_includedirs("include/")
    add_files("src/*.cc")
    add_packages("glfw", "glm", "glog", "glad", "glew", "folly")
target_end()
