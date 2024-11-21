rule("redscript")
    set_extensions(".reds")
    -- on_build_file(function (target, sourcefile, opt)
    --     -- not sure how to not hard-code the relative directory
    --     os.cp(sourcefile, path.join(target:targetdir() .. "/redscript", path.relative(sourcefile, "code/redscript")))
    -- end)
    on_install(function (target)
        for _, sourcebatch in pairs(target:sourcebatches()) do
            local sourcekind = sourcebatch.rulename
            if sourcekind == "redscript" then
                for _, sourcefile in ipairs(sourcebatch.sourcefiles) do
                    os.cp(sourcefile, path.join(target:installdir("launcher"), "mod", "assets", "redscript", path.relative(sourcefile, "code/redscript/assets")))
                end
            end
        end
    end)

target("redscript")
    if is_mode("debug") then
        add_defines("TP_REDSCRIPT_LOCATION=\"../../../../code/assets/redscript\"", {public = true})
    else 
        add_defines("TP_REDSCRIPT_LOCATION=\"assets/redscript\"", {public = true})
    end 
    set_kind("headeronly")
    set_group("Assets")
    add_rules("redscript")
    add_files("**.reds")