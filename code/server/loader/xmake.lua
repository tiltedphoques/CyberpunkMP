target("Server.Loader")
    set_kind("binary")
    set_group("Server")
    add_rules("csharp")
    add_files("Server.Loader.csproj")
    add_extrafiles("**.cs")

    add_deps("Server.Scripting", "Server.Native")