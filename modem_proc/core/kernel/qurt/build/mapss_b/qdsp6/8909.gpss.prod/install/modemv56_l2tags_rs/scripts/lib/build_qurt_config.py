#
# Build cust_config.o
#

def find_compiler(cfg):
    # Return the string which executes the compiler
    # Try the requested LLVM setting first, then try the opposite
    import os, subprocess
    cmds = []
    for try_llvm in (bool(cfg.use_llvm), not cfg.use_llvm):
        prog = ('hexagon-gcc','hexagon-clang')[try_llvm]
        paths = []
        if cfg.tools_path:
            if try_llvm:
                paths.append(os.path.join(cfg.tools_path,'Tools','bin'))
                paths.append(os.path.join(cfg.tools_path,'qc','bin'))
            else:
                paths.append(os.path.join(cfg.tools_path,'gnu','bin'))
        else:
            paths.append('')
        for p in paths:
            cmds.append(os.path.join(p,prog))
    for c in cmds:
        try:
            subprocess.Popen([c,'--version'],
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE).communicate()
            return c
        except OSError:
            pass
    raise Exception('Cannot find C compiler')

def build_qurt_config(cfg):
    import os, re, subprocess, sys

    # Do basic handling of the arguments
    compiler = find_compiler(cfg)
    if cfg.build_flags:
        build_flags = cfg.build_flags
    else:
        build_flags = []
    if cfg.asic:
        asic = ['-m%s' % cfg.asic]
    else:
        asic = []
    if cfg.object_path:
        object_path = cfg.object_path
    else:
        object_path = '.'
    cfile_path = cfg.cfile_path

    cust_config_o = cfg.object_path
    dir_path, cust_config_o_file_name = os.path.split(object_path)
    cust_config_main_f_name = cust_config_o_file_name.split(".")[0] + "_main.o"
    cust_config_island_f_name = cust_config_o_file_name.split(".")[0] + "_island.o"
    object_path_main = os.path.join(dir_path, cust_config_main_f_name)
    object_path_island = os.path.join(dir_path, cust_config_island_f_name)

    compiler_command = [compiler, '-g', '-nostdinc', '-O2', '-G0'] + asic + build_flags

    if '_main.' in os.path.basename(object_path):
        # Assume caller asked for the "main" (non-island) object only
        compiler_command += ['-DMAIN_ONLY',
                             '-o', object_path,
                             '-c', cfile_path]
    elif '_island.' in os.path.basename(object_path):
        # Assume caller asked for the "island" object only
        compiler_command += ['-DISLAND_ONLY',
                             '-o', object_path,
                             '-c', cfile_path]
    else:
        if '.' in os.path.basename(object_path):
            # Add the _main or the _island before the final . in the name
            object_path_main = '_main.'.join(object_path.rsplit('.',1))
            object_path_island = '_island.'.join(object_path.rsplit('.',1))
        else:
            # Add the _main or the _island after the name
            object_path_main = object_path+'_main'
            object_path_island = object_path+'_island'
        compiler_command = (compiler_command + ['-o', object_path, '-c', cfile_path, '&&'] +
                            compiler_command + ['-DMAIN_ONLY', '-o', object_path_main, '-c', cfile_path, '&&'] +
                            compiler_command + ['-DISLAND_ONLY', '-o', object_path_island, '-c', cfile_path])

    while '&&' in compiler_command:
        i = compiler_command.index('&&')
        sub_command = compiler_command[:i]
        compiler_command = compiler_command[i+1:]
        print ' '.join(sub_command)
        if subprocess.Popen(sub_command).wait():
            raise Exception('Unable to compile configuration object')

    print ' '.join(compiler_command)

    if subprocess.Popen(compiler_command).wait():
        raise Exception('Unable to compile configuration object')
