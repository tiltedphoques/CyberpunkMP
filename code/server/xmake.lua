target("Server")
    set_kind("binary")
    add_files("**.cpp")
    add_headerfiles("**.h", "**.hpp", "**.inl")

    set_pcxxheader("ServerPCH.h")
    add_includedirs(
        ".", 
        "../../build", 
        "../../vendor"
    )

    add_defines("STEAMNETWORKINGSOCKETS_STATIC_LINK")

    add_deps("Common", "Protocol", "Scripting")

    add_packages(
        "gamenetworkingsockets",
        "mimalloc",
        "spdlog",
        "hopscotch-map",
        "cryptopp",
        "glm",
        "openssl",
        "zlib",
        "nlohmann_json",
        "dotnet",
        "flecs",
        "entt",
        "microsoft-gsl")

    after_install(function (target)
        os.cp(path.join(target:installdir("bin"), "Server.exe"), path.join(target:installdir("launcher"), "server", "Server.exe"))
        os.cp(path.join(target:installdir("bin"), "nethost.*"), path.join(target:installdir("launcher"), "server"))
    end)