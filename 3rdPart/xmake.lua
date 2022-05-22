---@diagnostic disable: undefined-global

add_rules("mode.debug", "mode.release")

add_requires("glfw 3.3.6")

-- package("uchardet")
--     add_deps("cmake")
--     set_sourcedir(path.join("$(projectdir)", "3rdPart", "uchardet"))
--     on_install(function (package)
--         local configs = {}
--         table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
--         table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
--         table.insert(configs, "-DBUILD_BINARY=" .. "OFF")
--         import("package.tools.cmake").install(package, configs)
--     end)
--
--     on_test(function (package)
--         assert(package:has_cfuncs("uchardet_get_charset", {includes = "uchardet/uchardet.h"}))
--     end)
-- package_end()

target("Imgui")
    set_kind("static")
    set_languages("c11", "cxx11")
    add_includedirs("$(projectdir)/3rdPart/imgui", { public = true })
    add_includedirs("$(projectdir)/3rdPart/imgui/backends", {public = true})
    add_files("$(projectdir)/3rdPart/imgui/*.cpp")
    add_files("$(projectdir)/3rdPart/imgui/backends/imgui_impl_glfw.cpp")
    add_files("$(projectdir)/3rdPart/imgui/backends/imgui_impl_opengl3.cpp")

    add_packages("glfw")
target_end()

target("ImGuizmo")
    set_kind("static")
    set_languages("c11", "cxx17")
    add_deps("Imgui")

    add_includedirs("$(projectdir)/3rdPart/ImGuizmo", { public = true })
    add_files("$(projectdir)/3rdPart/ImGuizmo/*.cpp")
target_end()

target("IconFontCppHeaders")
    set_kind("headeronly")

    add_includedirs("$(projectdir)/3rdPart/IconFontCppHeaders/", { public = true })
target_end()

target("nameof")
  set_kind("headeronly")
  add_includedirs("$(projectdir)/3rdPart/nameof/include", { public = true })
target_end()
