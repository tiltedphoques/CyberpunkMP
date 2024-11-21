rule("input")
    set_extensions(".xml")
    on_install(function (target)
        for _, sourcebatch in pairs(target:sourcebatches()) do
            local sourcekind = sourcebatch.rulename
            if sourcekind == "input" then
                for _, sourcefile in ipairs(sourcebatch.sourcefiles) do
                    os.cp(sourcefile, path.join(target:installdir("launcher"), "mod", "assets", "Inputs", path.basename(sourcefile) .. ".xml"))
                end
            end
        end
    end)

target("Inputs")
    if is_mode("debug") then
        add_defines("TP_INPUTS_LOCATION=\"../../../../code/assets/Inputs/CyberpunkMP.xml\"", {public = true})
    else 
        add_defines("TP_INPUTS_LOCATION=\"assets/Inputs/CyberpunkMP.xml\"", {public = true})
    end 
    set_kind("headeronly")
    set_group("Assets")
    add_rules("input")
    add_files("CyberpunkMP.xml")