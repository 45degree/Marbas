---@diagnostic disable: undefined-global

add_rules("mode.debug", "mode.release")

includes("./Marbas.RHI")

target("ImGuizmo")
    set_kind("static")
    set_languages("c11", "cxx17")
    add_deps("Marbas.RHI")

    add_includedirs(path.join(os.scriptdir(), "ImGuizmo"), { public = true })
    add_files(path.join(os.scriptdir(), "ImGuizmo/*.cpp"))
target_end()

target("IconFontCppHeaders")
    set_kind("headeronly")

    add_includedirs(path.join(os.scriptdir(), "IconFontCppHeaders/"), { public = true })
target_end()

target("nameof")
  set_kind("headeronly")
  add_includedirs(path.join(os.scriptdir(), "nameof/include"), { public = true })
target_end()
