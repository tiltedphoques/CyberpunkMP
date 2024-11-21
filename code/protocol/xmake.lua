target("Protocol")
    set_kind("static")
    set_group("Libraries")
    add_rules("codegen")
    add_files("**.cpp")
    add_files("**.proto")
    add_extrafiles("**.proto")
    add_headerfiles("**.h", "**.hpp", "**.inl")
    set_policy("build.across_targets_in_parallel", false)

    set_pcxxheader("ProtocolPCH.h")
    add_includedirs(".", {public = true})
    add_includedirs(
        "../../build", 
        "../../vendor"
    )
    add_deps("Common")

    add_packages(
        "mimalloc",
        "spdlog",
        "hopscotch-map",
        "glm",
        "flecs",
        "entt")