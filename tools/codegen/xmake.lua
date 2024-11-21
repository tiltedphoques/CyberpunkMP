rule("codegen")
    set_extensions(".proto")

    on_load(function(target)
        target:add("deps", "NetPack")
    end)

    on_config(function(target)
        local sourcebatch = target:sourcebatches()["codegen"]
    
        for _, sourcefile in ipairs(sourcebatch.sourcefiles) do
            local outputSourceFile = path.join(target:autogendir(), "rules", "netpack", path.basename(sourcefile) .. ".gen.cpp")
            local objectfile = target:objectfile(outputSourceFile)
            table.insert(target:objectfiles(), objectfile)
        end

        target:add("includedirs", path.join(target:autogendir(), "rules", "netpack"), {public = true})
    end)

    before_buildcmd_file(function(target, batchcmds, sourcefile, opt)
        import("core.project.project")
		import("core.tool.toolchain")

        local netpack = target:dep("NetPack"):targetfile()

        -- add commands
		batchcmds:show_progress(opt.progress, "${color.build.object}compiling.netpack %s", sourcefile)
        local argv = {}

        local output_dir = path.join(target:autogendir(), "rules", "netpack")
        target:add("includedirs", output_dir, {public = true})

        table.insert(argv, sourcefile)
        table.insert(argv, output_dir)

        batchcmds:vexecv(netpack, argv, { curdir = "." })

		local outputHeaderFile = path.join(target:autogendir(), "rules", "netpack", path.basename(sourcefile) .. ".gen.h")
        local outputSourceFile = path.join(target:autogendir(), "rules", "netpack", path.basename(sourcefile) .. ".gen.cpp")

        local objectfile = target:objectfile(outputSourceFile)
        --table.insert(target:objectfiles(), objectfile)
        batchcmds:compile(outputSourceFile, objectfile)

		-- add deps
		batchcmds:add_depfiles(sourcefile)
		batchcmds:set_depmtime(os.mtime(outputHeaderFile))
		batchcmds:set_depcache(target:dependfile(outputHeaderFile))
        batchcmds:set_depcache(target:dependfile(outputSourceFile))
    end)