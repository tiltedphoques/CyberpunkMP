rule("tweak")
    set_extensions(".tweak")

    on_install(function (target)
        for _, sourcebatch in pairs(target:sourcebatches()) do
            local sourcekind = sourcebatch.rulename
            if sourcekind == "tweak" then
                for _, sourcefile in ipairs(sourcebatch.sourcefiles) do
                    os.cp(sourcefile, path.join(target:installdir("launcher"), "mod", "assets", "Tweaks", path.basename(sourcefile) .. ".tweak"))
                end
            end
        end
    end)

target("Tweaks")
    if is_mode("debug") then
        add_defines("TP_TWEAKS_LOCATION=\"../../../../code/assets/Tweaks/\"", {public = true})
    else 
        add_defines("TP_TWEAKS_LOCATION=\"assets/Tweaks/\"", {public = true})
    end 
    set_kind("headeronly")
    set_group("Assets")
    add_rules("tweak")
    add_files("**.tweak")