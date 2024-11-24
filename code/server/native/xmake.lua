target("Server.Native")
    set_kind("shared")
    set_group("Server")
    add_files("**.cpp")
    
    add_headerfiles("**.h", "**.hpp", "**.inl")
    set_pcxxheader("ServerPCH.h")
    add_includedirs(
        ".", 
        "../../../build", 
        "../../../vendor"
    )

    add_defines("STEAMNETWORKINGSOCKETS_STATIC_LINK")

    add_deps("Common", "Protocol", "Server.Scripting")

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
        "flecs",
        "entt",
        "microsoft-gsl")

    after_install(function (target)
        os.cp(path.join(target:installdir("bin"), "Server.Native.dll"), path.join(target:installdir("launcher"), "server", "Server.Native.dll"))
    end)