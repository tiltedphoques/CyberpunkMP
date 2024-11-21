rule("archive")
    set_extensions(".archive")
    -- on_build_file(function (target, sourcefile, opt)
    --     os.cp(sourcefile, path.join(target:targetdir(), path.basename(sourcefile) .. ".archive"))
    -- end)
    on_install(function (target)
        print("on_install")
        for _, sourcebatch in pairs(target:sourcebatches()) do
            local sourcekind = sourcebatch.rulename
            if sourcekind == "archive" then
                for _, sourcefile in ipairs(sourcebatch.sourcefiles) do
                    os.cp(sourcefile, path.join(target:installdir("launcher"), "mod", "assets", "Archives", path.basename(sourcefile) .. ".archive"))
                end
            end
        end
    end)

target("Archives")
    if is_mode("debug") then
        add_defines("TP_ARCHIVES_LOCATION=\"../../../../code/assets/Archives/packed/archive/pc/mod/\"", {public = true})
    else 
        add_defines("TP_ARCHIVES_LOCATION=\"assets/Archives\"", {public = true})
    end 
    set_kind("headeronly")
    set_group("Assets")
    add_rules("archive")
    add_files("packed/archive/pc/mod/CyberpunkMP.archive")