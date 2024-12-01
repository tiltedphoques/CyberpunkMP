add_requires("hopscotch-map", "snappy", "gamenetworkingsockets", "catch2 2.13.9", "libuv", "openssl", "spdlog")
add_requireconfs("*.protobuf*", { build = true })
add_requireconfs("mimalloc", {configs = {rltgenrandom = true}})

if is_plat("windows") then
    add_requires("minhook", "mem", "xbyak")
end

target("Common")
    set_kind("static")
    add_files("**.cpp")
    remove_files("Tests/**")
    set_group("Libraries")
    add_headerfiles("**.h", "**.hpp", "**.inl")

    set_pcxxheader("CommonPCH.h")
    add_includedirs(".", {public = true})
    add_includedirs(
        "../../build", 
        "../../vendor"
    )

    add_packages("hopscotch-map", "snappy", "gamenetworkingsockets", "libuv")
    if is_plat("windows") then
        add_packages("minhook", "mem", "xbyak")
    else
        remove_files("Reverse/**")
    end

    add_cxflags("-fPIC")
    add_defines("STEAMNETWORKINGSOCKETS_STATIC_LINK")

    add_packages(
        "spdlog",
        "glm",
        "hopscotch-map",
        "mimalloc",
        "gamenetworkingsockets",
        "snappy",
        "openssl",
        "libuv")
