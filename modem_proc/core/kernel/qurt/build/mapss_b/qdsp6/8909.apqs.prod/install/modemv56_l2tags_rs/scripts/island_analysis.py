#===============================================================================
#                    Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
#                         QUALCOMM Proprietary/GTDR
#===============================================================================
import os

#----------------------------------------------------------------------------
# Hooks for SCons
#----------------------------------------------------------------------------
def exists(env):
   return env.Detect('island_analysis')

def generate(env):
    island_analysis_generate(env)
    
def island_analysis_generate(env):
    env.AddMethod(island_analysis_builder_wrapper, "IslandAnalysis")
    island_analysis_bld = env.Builder(action = island_analysis_builder)
    env['BUILDERS']['IslandAnalysisInternalBuilder'] = island_analysis_bld

def island_analysis_builder_wrapper(env, target, source, libdirs=[], ignorelist=[], elf=[]):
    env.Replace(ISLAND_ANALYSIS_LIBDIRS = libdirs)
    env.Replace(ISLAND_ANALYSIS_IGNORELIST = ignorelist)
    env.Replace(ISLAND_ANALYSIS_ELF = elf)
    if elf:
       string = os.path.split(target)
       csv = os.path.join(string[0],"island.csv")
       env.SideEffect(csv,target)
       env.Replace(ISLAND_ANALYSIS_CSV = csv)
       env.Clean(csv,target)
    return env.IslandAnalysisInternalBuilder(target, source)

def island_analysis_builder(target, source, env):
    libdirs = env.get('ISLAND_ANALYSIS_LIBDIRS')
    ignorelist = env.get('ISLAND_ANALYSIS_IGNORELIST')
    elf = env.get('ISLAND_ANALYSIS_ELF')

    if env.get('LIST_EXTREF_PY') == None:
        env.Replace(LIST_EXTREF_PY = "${BUILD_ROOT}/core/kernel/qurt/scripts/list_extref.py")
    if int(env.get('HEXAGON_RTOS_RELEASE').split(".")[0]) >= 7:
        env.Replace(IA_OBJDUMP = "${QDSP6BIN}/hexagon-llvm-objdump${EXE_EXT}")
    else:
        env.Replace(IA_OBJDUMP = "${QDSP6BIN}/hexagon-objdump${EXE_EXT}")
    analysis_cmd = "python ${LIST_EXTREF_PY} -v -e ${IA_OBJDUMP} "
    for src in source:
        analysis_cmd += "-s "+str(src)+" "
    for libdir in libdirs:
        analysis_cmd += "-l "+libdir+" "
    for ilist in ignorelist:
        analysis_cmd += "-i "+ilist+" "

    if elf:
        csv = env.get('ISLAND_ANALYSIS_CSV')
        analysis_cmd += "-x "+str(elf)+" "
        analysis_cmd += "-c "+str(csv)+" "

    analysis_cmd = env.subst(analysis_cmd)
    analysis_cmd = analysis_cmd+" > "+str(target[0])

    print analysis_cmd
    #std_out, std_err, return_val = env.ExecCmds(analysis_cmd)
    os.system(analysis_cmd)
    #return target
