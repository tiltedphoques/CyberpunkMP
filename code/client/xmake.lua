add_requires("imgui", "xbyak", "minhook", "wil", "nameof", "semver")

target("Client")
    set_basename("CyberpunkMP")
    set_kind("shared")
    set_symbols("debug", "hidden")
    add_defines("_CRTDBG_MAP_ALLOC")
    set_runargs("--online")
    add_ldflags(
        "/FORCE:MULTIPLE",
        "/IGNORE:4254,4006",
        --"/DYNAMICBASE:NO",
        "/SAFESEH:NO",
        "/LARGEADDRESSAWARE",
        "/INCREMENTAL:NO",
        "/LAST:.zdata"
        --"/SUBSYSTEM:WINDOWS",
        --"/ENTRY:mainCRTStartup", { force = true }
        )
    add_includedirs(
        ".",
        "../",
        "../../build",
        "../../vendor/")
    set_pcxxheader("stdafx.h")
    add_headerfiles("**.h", "**.hpp", "**.inl")
    add_files(
        "**.cpp",
        "launcher.rc")
    add_links("ntdll_x64")
    add_linkdirs(".")
    add_syslinks(
        "user32",
        "version",
        "shell32",
        "comdlg32",
        "bcrypt",
        "ole32",
        "dxgi",
        "d3d12",
        "gdi32",
        "SetupAPI",
        "Powrprof",
        "Cfgmgr32",
        "Propsys",
        "delayimp")

    add_deps("Common", "Protocol", "RED4ext.SDK", "redscript", "Archives", "Inputs", "Tweaks")

    add_packages(
        "mimalloc",
        "spdlog",
        "hopscotch-map",
        "cryptopp",
        "gamenetworkingsockets",
        "imgui",
        "minhook",
        "mem",
        "openssl",
        "zlib",
        "xbyak",
        "glm",
        "nlohmann_json",
        "entt",
        "wil",
        "nameof",
        "semver",
        "microsoft-gsl",
        "flecs")

    on_install(function (target)
        os.cp(target:targetfile(), path.join(target:installdir("launcher"), "mod", "CyberpunkMP.dll"))
    end)
