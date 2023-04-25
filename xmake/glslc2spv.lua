---@diagnostic disable: undefined-global

rule('glslc2spv', function()
  set_extensions(
    '.vert',
    '.tesc',
    '.tese',
    '.geom',
    '.comp',
    '.frag',
    '.comp',
    '.mesh',
    '.task',
    '.rgen',
    '.rint',
    '.rahit',
    '.rchit',
    '.rmiss',
    '.rcall',
    '.glsl'
  )

  before_buildcmd_file(function(target, batchcmds, sourcefile, opt)
    import('lib.detect.find_tool')

    -- get glslc
    local glslc = find_tool('glslc')
    assert(glslc, 'glslc not found!')

    -- glsl to spv
    local targetenv = target:extraconf('rules', 'glslc2spv', 'targetenv') or 'vulkan1.0'
    local outputdir = target:extraconf('rules', 'glslc2spv', 'outputdir')
        or path.join(target:autogendir(), 'rules', 'glslc2spv')
    local includeDirs = target:get('includedirs')

    local spvfilepath = path.join(outputdir, path.filename(sourcefile) .. '.spv')
    batchcmds:show_progress(opt.progress, '${color.build.object}generating.glslc2spv %s', sourcefile)
    batchcmds:mkdir(outputdir)

    local config = {}
    table.insert(config, '--target-env=' .. targetenv)
    for i, v in ipairs(includeDirs) do
      table.insert(config, '-I')
      table.insert(config, path(v))
    end

    table.insert(config, '-o')
    table.insert(config, path(spvfilepath))
    table.insert(config, path(sourcefile))

    batchcmds:vrunv('glslc', config)

    local outputfile = spvfilepath

    -- add deps
    batchcmds:add_depfiles(sourcefile)
    batchcmds:add_depfiles(includeDirs)
    batchcmds:set_depmtime(os.mtime(outputfile))
    batchcmds:set_depcache(target:dependfile(outputfile))
  end)
end)
