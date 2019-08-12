from SCons.Script import *
import re
import subprocess
import signal
from threading import Timer

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('qurt_builders')

def generate(env):
   # load builder into enviroment
   cust_config_obj_generate(env)
   cust_config_src_generate(env)
   obj_dump_generate(env)
   sim_generate(env)
   test_sim_generate(env)
   test_island_analysis_generate(env)
   reloc_generate(env)
   qurt_image_generate(env)
   build_param_generate(env)
   config_vars_generate(env)
   const_autogen_generate(env)
   ppend_cust_config_generate(env)
   traptab_generate(env)
   qurt_program_generate(env)
   qurt_islandobject_generate(env)
   hex_cfg_generate(env)
   lcs_generate(env)
   qurt_object_scan_generate(env)
   return None

#------------------------------------------------------------------------------
# Customer Configuration definition
#------------------------------------------------------------------------------
def cust_config_obj_generate(env):
   CustConfigObjBuilder = Builder(action = 'python ${Q6_RTOS_ROOT}/scripts/qurt_config.py update ${USE_LLVM_OPT} -T ${Q6_TOOLS_ROOT} -o $TARGET $SOURCE',
                       emitter = cust_config_emitter,
                       suffix = '.o',
                       src_suffix = '.xml')
   env['BUILDERS']['CustConfigObj'] = CustConfigObjBuilder

def cust_config_src_generate(env):
   CustConfigSrcBuilder = Builder(action = 'python ${Q6_RTOS_ROOT}/scripts/qurt_config.py update --src_only -T ${Q6_TOOLS_ROOT} -o $TARGET $SOURCE',
                       emitter = cust_config_emitter,
                       suffix = '.c',
                       src_suffix = '.xml')
   env['BUILDERS']['CustConfigSrc'] = CustConfigSrcBuilder

def cust_config_emitter(target, source, env):
   # Add dependencies on scripts
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/Input/cust_config.c")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/Input/build_params.txt")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/__init__.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/build_qurt_config.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/ezxml.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/interrupt_xml.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/kernel_xml.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/machine_xml.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/merge.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/parse_build_params.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/parse_spec.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/physpool_xml.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/lib/program_xml.py")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/qurt_config.py")

   return (target, source)

#------------------------------------------------------------------------------
# Object Dump
#------------------------------------------------------------------------------
def obj_dump_generate(env):
   ObjDumpBuilder = Builder(action = '${OBJDUMP} ${OBJDUMPOPT} $SOURCE > $TARGET',
                       suffix = '.dump',
                       src_suffix = '.pbn')
   env['BUILDERS']['ObjectDump'] = ObjDumpBuilder

#------------------------------------------------------------------------------
# Simulator
#------------------------------------------------------------------------------
def sim_generate(env):
   SimBuilder = Builder(action = "${SIM} -S $TARGET --simulated_returnval $SOURCE ${SIM_RTOS} ${SYMFILE} ${SIM_COSIM} ${SIMOUT} ${SIMERR}",
                       suffix = '.txt')
   env['BUILDERS']['Simulator'] = SimBuilder

#------------------------------------------------------------------------------
# Simulator for testing, timeout after
#------------------------------------------------------------------------------
def test_sim_generate(env):
    TestSimBuilder = Builder(action = test_sim_builder)
    env['BUILDERS']['TestSimulator'] = TestSimBuilder

def kill_proc(proc, timeout):
    timeout["value"] = True
    if ((sys.version_info[0] == 2) and (sys.version_info[1] >= 6)) or (sys.version_info[0] > 2):
        proc.kill()
    else:
        os.kill(proc.pid, signal.SIGKILL)

def test_sim_builder(target, source, env):
    if env.subst('$USE_LLVM') == 'True':
       cmd = "%s -S %s --simulated_returnval %s --rtos %s --symfile %s --cosim_file %s %s" % (env.subst("${SIM}"),
          str(target[1]), str(source[0]), str(source[1]), str(source[2]), str(source[3]), (env.subst("${HEXAGON_MACHINE}")))
    else:
        cmd = "%s -S %s --simulated_returnval %s --rtos %s --symfile %s --cosim_file %s" % (env.subst("${SIM}"),
          str(target[1]), str(source[0]), str(source[1]), str(source[2]), str(source[3]))
    print cmd;
    command = cmd.split()

    simproc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    timeout = {"value": False}
    timer = Timer(float(env.subst("${TEST_TIMEOUT}")), kill_proc, [simproc, timeout])
    timer.start()
    stdout, stderr = simproc.communicate()
    timer.cancel()

    outfile = open(str(target[2]), 'w+')
    outfile.write(stdout)
    outfile.write(stderr)
    outfile.close

    if str(timeout["value"]) == "True":
        return "TIMEOUT"
    else:
        file = open(str(target[0]), 'w+')
        if simproc.returncode == 0:
            file.write('PASS')
        else:
            file.write('FAIL')
        file.close
        return simproc.returncode

#------------------------------------------------------------------------------
# Simulator for testing, timeout after
#------------------------------------------------------------------------------
def test_island_analysis_generate(env):
    TestIslandAnalysisBuilder = Builder(action = test_island_analysis_builder)
    env['BUILDERS']['TestIslandAnalysis'] = TestIslandAnalysisBuilder

def test_island_analysis_builder(target, source, env):
    size_threshold = env.get('ISLAND_THRESHOLD')
    f = open(str(source[0]), 'r')
    analysis = f.read()
    f.close()

    size = "65535"
    extref = "0"
    for analysisline in analysis.split("\n"):
        undefpattern = re.compile('Total Undefined: (.+){1}')
        sizepattern = re.compile('Total: (.+){1} kb')
        sizematch = sizepattern.match(analysisline)
        undefmatch = undefpattern.match(analysisline)
        if(sizematch):
            size = sizematch.group(1)
        if(undefmatch):
            extref = undefmatch.group(1)

    fail=False
    analysis_result="${BUILD_DIR}/analysis_result"
    file = open(str(target[0]), 'w+')
    if ((float(size) <= float(size_threshold)) and (int(extref) == 0)):
        file.write('PASS')
    else:
        file.write('FAIL')
        fail=True
    file.close

    file = open(str(target[1]), 'w+')
    file.write("Full Island Analysis file found at:\n")
    file.write(os.path.abspath(str(source[0])))
    file.write("\n\nIsland size is "+size+" kb\n")
    if (float(size) > float(size_threshold)):
        file.write("***EXCEEDS THRESHOLD***\n")
    file.write("\nUnaccounted external references: "+extref+"\n")
    if (int(extref) != 0):
        file.write("***EXCEEDS THRESHOLD***\n")
    file.close

    if (fail):
        return -1

#------------------------------------------------------------------------------
# hex cfg
#------------------------------------------------------------------------------
def hex_cfg_generate(env):
    HexCfgBuilder = Builder(action = hex_cfg_builder)
    env['BUILDERS']['HexCfg'] = HexCfgBuilder

def hex_cfg_builder(target, source, env):
    timerlib = env.subst('$TimerLib')
    l2viclib = env.subst('$L2vicLib')
    srcfile = open(str(source[0]),'r')
    tempstr = srcfile.read()
    srcfile.close()

    tgtfile = open(str(target[0]),'w+')
    tempstr = tempstr.replace('qtimer.so', timerlib)
    tempstr = tempstr.replace('l2vic.so', l2viclib)
    tgtfile.write(tempstr)
    tgtfile.close()

#------------------------------------------------------------------------------
# Relocation
#------------------------------------------------------------------------------
def reloc_generate(env):
   RelocBuilder = Builder(action = "${OBJCOPY} --change-start +${RELOC_ADDR} " +
                                 "--change-section-lma .start+${RELOC_ADDR} " +
                                 "--change-section-lma .init+${RELOC_ADDR} " +
                                 "--change-section-lma .text+${RELOC_ADDR} " +
                                 "--change-section-lma .fini+${RELOC_ADDR} " +
                                 "--change-section-lma .rodata+${RELOC_ADDR} " +
                                 "--change-section-lma .data+${RELOC_ADDR} " +
                                 "--change-section-lma .eh_frame+${RELOC_ADDR} " +
                                 "--change-section-lma .ctors+${RELOC_ADDR} " +
                                 "--change-section-lma .dtors+${RELOC_ADDR} " +
                                 "--change-section-lma .bss+${RELOC_ADDR} " +
                                 "--change-section-lma .sdata+${RELOC_ADDR} " +
                                 "--change-section-lma .sbss+${RELOC_ADDR} " +
                                 "$SOURCE $TARGET")
   env['BUILDERS']['Relocate'] = RelocBuilder

#------------------------------------------------------------------------------
# Qurt Image
#------------------------------------------------------------------------------
#guest os image needs to be first in source list
def qurt_image_generate(env):
   QurtImageBuilder = Builder(action = "python ${Q6_RTOS_ROOT}/scripts/qurt-image-build.py $SOURCES -o $TARGET -p ${PADDR} -t ${Q6_TOOLS_ROOT}")
   env['BUILDERS']['QurtImage'] = QurtImageBuilder

#------------------------------------------------------------------------------
# Builder: Deriving build_params.txt
#------------------------------------------------------------------------------
def build_param_generate(env):
   build_param_bld = Builder(action = Action(gen_build_param, cmdstr='Building ${TARGET} ...'), emitter=gen_build_emitter)
   env['BUILDERS']['build_param_builder'] = build_param_bld

def gen_build_emitter(target, source, env):
   # Ignore any sources specified; they were never used anyway.
   #
   # There is only one source needed, and it's a SCons Value node
   #  containing the string contents of the output file.
   #
   # This has the desired effect of rebuilding the output file
   #  any time that its contents would change, and leaving it
   #  unchanged otherwise.
   #
   contents  = ("Q6VERSION="         +env.subst('$Q6VERSION')+"\n")
   contents += ("BUILD_CONFIG_FLAGS="+env.subst('$BUILD_CONFIG_FLAGS')+"\n")
   contents += ("Q6_TOOLS_ROOT="     +os.path.realpath(env.subst('$Q6_TOOLS_ROOT'))+"\n")
   return target, [env.Value(env.subst(contents))]

def gen_build_param(target, source, env):
   target = str(target[0])
   fileHandle = open (target, 'w')
   fileHandle.write(source[0].get_contents())
   fileHandle.close()

#------------------------------------------------------------------------------
# Builder: Prepending cust_config.c
#------------------------------------------------------------------------------
def ppend_cust_config_generate(env):
   cust_config_bld = Builder(action = ppend_cust_config)
   env['BUILDERS']['cust_config_builder'] = cust_config_bld

def ppend_cust_config(target, source, env):
   target = str(target[0])
   tmplt  = str(source[0])
   mcr    = str(source[1])

   fileTarget = open (target, 'w')
   fileTmplt  = open (tmplt, 'r')
   fileMcr    = open (mcr, 'r')

   for line in fileMcr:
      if line.find('THREAD_CONTEXT_TOTALSIZE') != -1:
         fileTarget.write(line)
      elif line.find('QURT_CONFIG_MAX_HTHREAD') != -1:
         fileTarget.write(line)

   for line in fileTmplt:
      fileTarget.write(line)

   fileTarget.close()

#------------------------------------------------------------------------------
# Builder: qurt_config_vars.h
#------------------------------------------------------------------------------
def config_vars_generate(env):
   env['BUILDERS']['ConfigVarsGen'] = Builder(action = config_vars_builder)

def config_vars_builder(target, source, env):
    arglist = env.subst("$CC -E -P -DGEN_CONFIG_HEADER").split(" ")
    for w in source:
        if re.match('.*\\.h', str(w)):
            arglist += ["-include", str(w)]
    for w in source:
        if re.match('.*\\.c', str(w)):
            arglist += [str(w)]
    outinc_file = open(str(target[0]), 'w')
    outinc_file.write('/* This file is automatically generated. */\n\n')
    outinc_file.write('#ifndef GEN_CONFIG_HEADER_USED\n')
    outinc_file.write('#define GEN_CONFIG_HEADER_USED\n')
    outinc_file.flush()
    print arglist
    cpp = subprocess.Popen(arglist, stdout=outinc_file)
    val = cpp.wait()
    if val == 0:
        outinc_file.write('#endif /* GEN_CONFIG_HEADER_USED */\n')
    outinc_file.close()
    if val != 0:
        os.remove(str(target[0]))
    return val


#------------------------------------------------------------------------------
# Builder: qurt_config_vars.h
#------------------------------------------------------------------------------
def const_autogen_generate (env):
   import os
   env['QURTNUL'] = os.devnull
   env['BUILDERS']['const_autogen_builder'] = Builder(action = const_autogen_builder)

def const_autogen_builder(target, source, env):
    arglist = env.subst("${SIM} --quiet --pmu_statsfile $QURTNUL --statsfile $QURTNUL "+str(source[0])).split(" ")
    outinc_file = open(str(target[0]), 'w')
    print arglist
    cpp = subprocess.Popen(arglist, stdout=outinc_file)
    val = cpp.wait()
    outinc_file.close()
    if val != 0:
        os.remove(str(target[0]))
    return val
#------------------------------------------------------------------------------
# xml config with paddr option
#------------------------------------------------------------------------------
def qurt_image_generate(env):
   QurtImageBuilder = Builder(action = "python ${Q6_RTOS_ROOT}/scripts/qurt-image-build.py $SOURCES -o $TARGET -p ${PADDR} -t ${Q6_TOOLS_ROOT}")
   env['BUILDERS']['QurtImage'] = QurtImageBuilder

#------------------------------------------------------------------------------
# traptab builder
#------------------------------------------------------------------------------
def traptab_generate(env):
   env['BUILDERS']['TrapTabGen'] = Builder(generator = traptab_generator)
def traptab_generator(source, target, env, for_signature):
   return ' '.join(['python ', str(source[0]),'-outtab', str(target[0]),'-outinc', str(target[1]),env['CC'],env.subst('$CCFLAGS'),' '.join("-I%s" % w for w in env['CPPPATH']),'-E -x c', str(source[1])])

#------------------------------------------------------------------------------
# QuRT program builder (tricked out replacement for Program)
#
# Allows direct specification of XML files and linker scripts as sources
#  to the QurtProgram.  Also handles a lot of the details regarding how
#  to invoke the linker correctly.
# Installs dependencies on the standard QuRT and tools library files.
#------------------------------------------------------------------------------
def qurt_program_generate(env):
   def qurt_program_generator(source, target, env, for_signature):
      control = env['QPCONTROL']
      cmd = [control['LINK'],
             [control.get('LINKFLAGS',[])],
             '-m'+env['Q6VERSION'],
             '-g',
             control['SMALLDATA'],
             '-nostdlib',
             [['--island-spec', str(source[i])] for i in control['SPECSRCS']],
             [['-T', str(source[i])] for i in control['CMDSRCS']],
             [['--section-start', s] for s in Flatten([control.get('SECTION_START',[])])],
             [['-Map', str(e)] for e in target[1:]],
             '--start-group',
             [str(source[i]) for i in control['OBJSRCS']],
             '--end-group',
             '-o', str(target[0])]
      return ' '.join(Flatten(cmd))
   def qurt_program_emitter(target, source, env):
      if len(target) != 1:
         raise Exception("QurtProgram needs a single target")
      target_name = str(target[0])
      split_target = os.path.splitext(target_name)
      if not split_target[1]:
         target_name += '.elf'
      control = env['QPCONTROL']
      ntargets = [target_name]
      nsources = []
      specsrcs = []
      objsrcs = []
      cmdsrcs = []
      # If PRELINK is set in the instructions,
      #  set up some defaults
      if control.get('PRELINK', False):
         control.setdefault('MAPFILE', None)
         control.setdefault('AUTOLIB', False)
         control['LINKFLAGS'] = ['-r', '-d'] + control.get('LINKFLAGS', [])
      # Add map file to the target list
      #  Caller can disable map file creation by specifying MAPFILE=None
      if not 'MAPFILE' in control:
         # Caller didn't specify a map file name, use the target with a .map extension
         ntargets += [split_target[0]+'.map']
      else:
         mapfile = control['MAPFILE']
         if mapfile:
            # Caller specified a map file
            ntargets += [mapfile]
      psource = source
      if control.get('USER', False) and not control.get('PIE', False):
         # Caller requested to build a user mode ELF.
         smalldata = []
         adjenv = env
      else:
         # Caller requested to build a QuRT ELF.  Pass '-G 0'
         smalldata = ['-G', '0']
         # TBD - Add G0 to library paths just for this build
         adjenv = env.Clone(QDSP6_RELEASE_LIB_DIR = os.path.join(env['QDSP6_RELEASE_LIB_DIR'], 'G0'),
                            LIB_DSP_ROOT = os.path.join(env['LIB_DSP_ROOT'], 'G0'))
      if not 'AUTOLIB' in control or control['AUTOLIB']:
         # Caller didn't disable automatic libraries.  Add them here
         psource = Flatten([adjenv.subst(env['QDSP6OBJS1'])])
         psource += Flatten([adjenv.subst(env['QDSP6OBJS3'])])
         psource += source
         psource += Flatten([adjenv.subst(env['QDSP6OBJS4'])])
      else:
         psource = source
      if 'USER' in control and control['USER']:
         # Caller requested to build a user mode ELF.
         # Remove libqurtkernel
         psource = [x for x in psource if os.path.basename(str(x)) != 'libqurtkernel.a']
         # Change crt0 to crt1
         psource = [os.path.basename(str(x)) == 'crt0.o' and str(x).replace('crt0.o','crt1.o') or x for x in psource]
      elif 'KERNELSEP' in control and control['KERNELSEP']:
         # Caller requested kernel separation
         # Change crt0 to qurtkernel
         psource = [os.path.basename(str(x)) == 'crt0.o' and str(x).replace('crt0.o','qurtkernel.o') or x for x in psource]
      control['LINK'] = env['LINK']
      if 'ISLAND' in control and control['ISLAND']:
         # Caller requested island mode.
         # Replace libqurtkernel with libqurtkernel_main & libqurtkernel_island
         findkernel = [j for j in range(len(psource)) if os.path.basename(str(psource[j])) == 'libqurtkernel.a']
         if findkernel:
            libkernel = str(psource.pop(findkernel[0]))
            psource.insert(findkernel[0], libkernel[:-2]+'_main.a')
            psource.insert(findkernel[0], libkernel[:-2]+'_island.a')
         control['LINK'] = ['python', '${Q6_RTOS_ROOT}/scripts/qurt-island-link.py', control['LINK']]
         nsources += ['${Q6_RTOS_ROOT}/scripts/qurt-island-link.py']
      # If libqurt is in the list after libc, move it before libc
      libc_search = [n for n in range(len(psource)) if os.path.basename(str(psource[n])) == 'libc.a']
      libqurt_search = [n for n in range(len(psource)) if os.path.basename(str(psource[n])) == 'libqurt.a']
      if libc_search and libqurt_search and libc_search[0] < libqurt_search[0]:
         libqurt = psource.pop(libqurt_search[0])
         psource.insert(libc_search[0], libqurt)
      if control.get('PIE', False):
         # User requested PIC processing
         # Add -pie to linker flags
         control['LINKFLAGS'] = control.get('LINKFLAGS', []) + ['-pie', '-fpic']
         control['SECTION_START'] = []
         newsources = []
         for src in psource:
            compo = os.path.split(env.GetBuildPath(src))
            maybe = os.path.join(compo[0], 'pic', compo[1])
            if maybe.endswith('.o'):
               maybe = maybe[:-2] + 'S.o'
            if os.path.isfile(maybe):
               newsources.append(maybe)
            elif 'INSTALL_DIR' in env and maybe.startswith(env['INSTALL_DIR']):
               newsources.append(maybe)
            else:
               newsources.append(src)
         psource = newsources
      for src in psource:
         srcname = str(src)
         ext = os.path.splitext(srcname)[1]
         if ext == '.c' or ext == '.cpp' or ext == '.s' or ext == '.S':
            # It's a C file that needs to be compiled
            objsrcs += [len(nsources)]
            nsources += [env.Object(src)]
         elif ext == '.xml' or ext == '.cfg':
            # It's a configuration file that needs to be built
            #  Assumes that cust_config.o is available as an object file name
            if 'ISLAND' in control and control['ISLAND']:
               objsrcs += [len(nsources)]
               nsources += [env.CustConfigObj('cust_config_main.o', src)]
               objsrcs += [len(nsources)]
               nsources += [env.CustConfigObj('cust_config_island.o', src)]
            else:
               objsrcs += [len(nsources)]
               nsources += [env.CustConfigObj('cust_config.o', src)]
         elif ext == '.cmd' or ext == '.lcs':
            # It's a linker command script
            cmdsrcs += [len(nsources)]
            nsources += [src]
         elif ext == '.ispec':
            # It's an island spec file
            specsrcs += [len(nsources)]
            nsources += [src]
         else:
            # It's not an extension recognized above; it's probably .o or .a; pass it directly to linker
            objsrcs += [len(nsources)]
            nsources += [src]
      control['OBJSRCS'] = objsrcs
      control['CMDSRCS'] = cmdsrcs
      control['SPECSRCS'] = specsrcs
      control['SMALLDATA'] = smalldata
      return ntargets, nsources
   def qurt_program_method(env, target, source, **varg):
      # target should be the ELF file to be generated
      # source should be a list of input files
      # varg is a dictionary containing the addition named arguments
      return env.Clone(QPCONTROL=varg).QurtProgramExt(target, source)
   env['BUILDERS']['QurtProgramExt'] = Builder(generator=qurt_program_generator, emitter=qurt_program_emitter)
   env.AddMethod(qurt_program_method, 'QurtProgram')

def qurt_islandobject_generate(env):
   #
   # The IslandObject() builder is an "extension" of the Object()
   #  builder, with the following changes:
   # If the named parameter ISLAND_SELECT=True, then only those
   #  source files which include the string INCLUDE_MAIN_CONTENTS or INCLUDE_ISLAND_CONTENTS
   #  are passed through, and their output object files are given the suffix
   #  _island.o rather than the default .o; in addition, the files are compiled
   #  with the define -DISLAND_ONLY.
   # If the named parameter ISLAND_SELECT=False, then all files
   #  are passed through; the files are compiled with the define -DMAIN_ONLY.
   #
   # See the qurtos directory for an example of how this works to build
   #  both normal and island variations of a single source file.
   #
   def qurt_islandobject_method(env, source, **varg):
      env = env.Clone()
      if varg.get('ISLAND_SELECT', False):
         env.Append(CPPDEFINES = 'ISLAND_ONLY')
         env.Replace(OBJSUFFIX = '_island.o')
         source = [s for s in source if qurt_islandobject_check(env, s)]
      else:
         env.Append(CPPDEFINES = 'MAIN_ONLY')
      return env.Object(source)
   def qurt_islandobject_check(env, s):
      f = open(env.GetBuildPath(s), 'r')
      x = f.read()
      f.close()
      return (('INCLUDE_MAIN_CONTENTS' in x) or ('INCLUDE_ISLAND_CONTENTS' in x))
   env.AddMethod(qurt_islandobject_method, 'IslandObject')

def lcs_generate (env):
   env['BUILDERS']['lcsbuilder'] = Builder(action = lcs_builder)

def lcs_builder(target, source, env):
    file_list = []
    for file in source[2:]:
        file_list += str(file)

    import imp
    qurtisland = imp.load_source('qurtisland', env.subst("${Q6_RTOS_ROOT}/scripts/qurt-island-link.py"))
    qurtisland.create_lcs(str(target[0]), str(source[0]), file_list, str(source[1]))


def qurt_object_scan_generate(env):
   qurt_object_scan_action = 'python $SOURCES $QURT_SCAN_FLAGS -T $Q6_TOOLS_ROOT -o $TARGET'

   def qurt_object_scan_emitter(target, source, env):
      # Emitter just adds the Python script at the *front* of the sources list.
      # This does triple duty of making the action command above get parsed
      #  correctly, getting the right path to the Python script, and creating
      #  a dependency on the Python script.
      return target, ['$INSTALL_SCRIPTS/qurt-object-scan.py']+source

   env['BUILDERS']['QurtObjectScan'] = Builder(action = qurt_object_scan_action,
                                               emitter = qurt_object_scan_emitter)

