add_requires("cxxopts")

target("NetPack")
    set_kind("binary")
    add_files("**.cpp", "**.cc")
    set_group("Tools")
    add_headerfiles("**.h", "**.hpp", "**.inl")
    set_policy("build.across_targets_in_parallel", false)

    set_pcxxheader("NetPackPCH.h")
    add_includedirs(
        ".", 
        "../../build", 
        "../../vendor"
    )

    add_deps("Common")

    add_packages(
        "mimalloc",
        "spdlog",
        "hopscotch-map",
        "cxxopts",
        "protobuf-cpp")