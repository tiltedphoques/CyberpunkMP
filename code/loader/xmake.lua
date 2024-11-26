target("Cyberpunk2077")
    set_kind("binary")
    set_basename("Cyberpunk2077")
    set_runargs("--online", "--skipStartMenu", "--ip 127.0.0.1")
    set_targetdir("F:\\Games\\SteamLibrary\\steamapps\\common\\Cyberpunk 2077\\bin\\x64\\")
    on_build(function(target) end)
    on_clean(function(target) end)
    on_link(function(target) end)
    after_build(function(target) 
        local targetdir = target:targetdir()
        local clientdir = path.join(os.projectdir(), target:dep("Client"):targetdir())

        local basedir = path.join(targetdir, "..\\..\\")
        local red4extPlugins = path.join(basedir, "red4ext\\plugins")

        local modPath = path.join(red4extPlugins, "zzzCyberpunkMP")

        if os.exists(modPath) then
            if os.isdir(modPath) then
                os.rmdir(modPath)
            else
                os.rm("zzzCyberpunkMP")
            end
        end

        os.mkdir(modPath)
        
        -- Define paths for the Client.dll and symlink
        local client_dll_path = path.join(clientdir, "CyberpunkMP.dll")
        local symlink_path = path.join(modPath, "CyberpunkMP.dll")

        os.ln(client_dll_path, symlink_path)
    end)

    add_deps("Client")