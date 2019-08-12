import glob, itertools, os, string, sys, subprocess, re, pickle, shutil, datetime, atexit, platform, stat

def write_setenv_files(static_variables, update_variables):
    if platform.system() == "Linux":
        setenv_cmd = open('setenv.sh', 'w')

        for var, val in static_variables.iteritems():
            setenv_cmd.write('export %s=%s\n' % (var, val))
        for var, val in update_variables.iteritems():
            val = string.replace(val, ";", ":")
            setenv_cmd.write('export %s=%s$%s\n' % (var, val, var))

        setenv_cmd.close()
    else:
        setenv_cmd = open('setenv.cmd', 'w')

        for var, val in static_variables.iteritems():
            setenv_cmd.write('set %s=%s\n' % (var, val))
        for var, val in update_variables.iteritems():
            setenv_cmd.write('set %s=%s%%%s%%\n' % (var, val, var))

        setenv_cmd.close()
        
def write_setenv_files_from_environment():
    if platform.system() == "Linux":
        setenv_cmd = open('setenv.sh', 'w')
    else:
        setenv_cmd = open('setenv.cmd', 'w')

    write_env_variable_to_setenv(     'ARMINC', setenv_cmd)
    write_env_variable_to_setenv(     'ARMLIB', setenv_cmd)
    write_env_variable_to_setenv(     'ARMBIN', setenv_cmd)
    write_env_variable_to_setenv(    'ARMPATH', setenv_cmd)
    write_env_variable_to_setenv( 'ARMINCLUDE', setenv_cmd)
    write_env_variable_to_setenv(   'ARMTOOLS', setenv_cmd)
        
    setenv_cmd.close()
    
def write_env_variable_to_setenv( env_var, setenv_cmd ):
    val = os.environ.get( env_var, 'none')
    if( val == 'none' ):
        print "ERROR: Environment variable " + env_var + " not defined."
        print "  Delete environment variable ARMPATH before running this build"
        print "  command to have it auto-detect the compiler settings"
        print "Exiting."
        setenv_cmd.close()
        sys.exit()
    else:
        if platform.system() == "Linux":
            setenv_cmd.write('export %s=%s\n' % (env_var, val))
        else:
            setenv_cmd.write('set %s=%s\n' % (env_var, val))
        
def get_armcc_version(path):
    if not os.path.exists(path):
        return

    armcc = subprocess.Popen(path, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if 0 != armcc.wait():
        print 'Could not run armcc from %s.' % path
        return
    output = armcc.stdout.read() + armcc.stderr.read()
    m = re.search(r'ARM(/Thumb)? C/C\+\+ Compiler, (RVCT)?(?P<major>\d+)\.(?P<minor>\d+) \[Build (?P<build>\d+)\]', output)
    if not m:
        print 'Could not identify version of armcc from %s.' % path
        return

    major = int(m.group('major'))
    minor = int(m.group('minor'))
    build = int(m.group('build'))
    return {'major' : major, 'minor' : minor, 'build' : build, 'path' : path, 'pretty_name' : m.group(0)}

def select_armcc_install(install_paths):
    selected = {}
    for path in install_paths:
        # pass the armcc path
        if platform.system() == "Linux":
            armcc_path = os.path.join(path, 'bin', 'armcc')
        else:
            armcc_path = os.path.join(path, 'bin', 'armcc.exe')

        # check the armcc version
        pending_armcc = get_armcc_version(armcc_path)
        if not pending_armcc:
            continue

        # restore the "base" path for later use
        pending_armcc['path'] = path

        if ((pending_armcc['major'] == 5) and (pending_armcc['minor'] == 1) and (pending_armcc['build'] == 94)):
            selected = pending_armcc

	#if (pending_armcc['build'] <= 94):
        #    if ((pending_armcc['major'] > selected.get('major', 0)) or
        #        (pending_armcc['major'] == selected['major'] and pending_armcc['minor'] > selected['minor']) or
        #        (pending_armcc['major'] == selected['major'] and pending_armcc['minor'] == selected['minor'] and pending_armcc['build'] > selected['build'])):
        #        selected = pending_armcc
    return selected

def setup_armcc_install(install_info):
    incpath = os.path.join(install_info['path'], 'include')
    armpath = os.path.join(install_info['path'], 'bin')

    static_variables = {
        # Lie to the build scripts to tell them to treat this like RVCT 4.1 until they officially support ARMCC 5.
        'ARMTOOLS'   : 'RVCT41',
        'ARMLIB'     : os.path.join(install_info['path'], 'lib'),
        'ARMINCLUDE' : incpath,
        'ARMINC'     : incpath,
        'ARMPATH'    : armpath,
        'ARMBIN'     : armpath,
    }

    update_variables = {
        'PATH'       : '.;' + os.path.join('..', 'tools', 'build', 'scons', 'SCons') + ';' + armpath + ';',
    }

    os.environ.update(static_variables)
    for var, val in update_variables.iteritems():
        current = os.environ.get(var)
        os.environ[var] = val + current

    write_setenv_files(static_variables, update_variables)

def select_rvct_install(install_paths):
    builds_paths = list(itertools.chain(*[glob.glob(path + '\\*') for path in install_paths]))
  
    selected = {}
    for path in builds_paths:
        # pass the armcc path
        armcc_path = os.path.join(path, 'win_32-pentium', 'armcc.exe')

        # check the armcc version
        pending_armcc = get_armcc_version(armcc_path)
        if not pending_armcc:
            continue

        #if ((pending_armcc['major'] > selected.get('major', 0)) or
        #    (pending_armcc['major'] == selected['major'] and pending_armcc['minor'] > selected['minor']) or
        #    (pending_armcc['major'] == selected['major'] and pending_armcc['minor'] == selected['minor'] and pending_armcc['build'] > selected['build'])):
        #    selected = pending_armcc
        if ((pending_armcc['major'] == 5) and (pending_armcc['minor'] == 1) and (pending_armcc['build'] == 94)):
            selected = pending_armcc

    if not selected:
        return
    selected['path'] = os.path.dirname(os.path.dirname((selected['path'])))

    # print warnings about stupid RVCT installs
    fs_build = int(os.path.basename(selected['path']))
    if fs_build != selected['build']:
        print '\n\nWarning: you have a broken RVCT install (filesystem build %s, reported build %s).' % (fs_build, selected['build'])
        print 'I will do my best to figure out how to proceed, but I cannot guarantee proper operation.\n\n'

    # try to find the matching data
    selected['prog_path'] = selected['path']
    build_data = selected['path'].split(os.sep)
    build_data[-3] = 'Data'
    selected['data_path'] = os.sep.join(build_data)

    if not os.path.exists(selected['data_path']):
        print '\n\n*** WARNING: you have a *seriously* broken RVCT install (corresponding data folder does not exist)'
        print 'Searching for the closest matching data folder; all bets are off at this point.\n\n'
        data_path_pattern = selected['data_path'].split(os.sep)
        data_path_pattern[-1] = '*'
        data_path_pattern = os.sep.join(data_path_pattern)
        data_paths = glob.glob(data_path_pattern)
        max_fs_build = max([int(data_path.split(os.sep)[-1]) for data_path in data_paths])
        data_path = selected['data_path'].split(os.sep)
        data_path[-1] = str(max_fs_build)
        selected['data_path'] = os.sep.join(data_path)
    return selected

def setup_rvct_install(install_info):
    version      = str(install_info['major']) + str(install_info['minor'])
    armtools     = 'RVCT'  + version
    alt_armtools = 'ARMCC' + version
    armlib       = os.path.join(install_info['data_path'], 'lib')
    arminc       = os.path.join(install_info['data_path'], 'include', 'windows')
    armpath      = os.path.join(install_info['prog_path'], 'win_32-pentium')
    armhome      = os.sep.join(install_info['prog_path'].split(os.sep)[:-4]),

    static_variables = {
        'ARMTOOLS'            : armtools,
        'ARMBUILD'            : str(install_info['build']),
        'ARMLIB'              : armlib,
        armtools + 'LIB'      : armlib,
        alt_armtools + 'LIB'  : armlib,
        'ARMINCLUDE'          : arminc,
        'ARMINC'              : arminc,
        armtools + 'INC'      : arminc,
        alt_armtools + 'INC'  : arminc,
        'ARMPATH'             : armpath,
        'ARMBIN'              : armpath,
        armtools + 'BIN'      : armpath,
        alt_armtools + 'BIN'  : armpath,
        'ARMHOME'             : armhome,
        'ARMROOT'             : armhome,
    }

    update_variables = {
        'PATH'                : '.;' + os.path.join('..', 'tools', 'build', 'scons', 'SCons') + ';' + armpath + ';',
    }

    os.environ.update(static_variables)
    for var, val in update_variables.iteritems():
        current = os.environ.get(var)
        os.environ[var] = val + current

    write_setenv_files(static_variables, update_variables)

compiler_types = [
    {
        'name'             : 'ARM Compiler 5',
        'paths'            : ['C:\\Apps\\ARMCT5*\\*',
                              '/usr/local/packages/qct/software/arm/RVDS/*' ],
        'install_selector' : select_armcc_install,
        'setup'            : setup_armcc_install,
    },

    {
        'name'             : 'RVCT 4.x',
        'paths'            : ['C:\\apps\\RV[CD][TS]*\\RVCT\\Programs\\*'],
        'install_selector' : select_rvct_install,
        'setup'            : setup_rvct_install,
    },
]

def find_compiler():
    print '\n\n\nAuto-detect your build environment...'
    
    for compiler in compiler_types:
        paths = list(itertools.chain(*[glob.glob(path) for path in compiler['paths']]))
        if not paths:
            print 'Did not find any installs of', compiler['name']
            continue

        install = compiler['install_selector'](paths)
        if not install:
            print 'Found no usable installs of', compiler['name']
            continue
        break

    compiler['setup'](install)
    print 'Set up to use', install['pretty_name']
    return True

    print 'Failed to find a usable toolchain.'
    return False

def dump_env(log):
  log.write('---------------------------------------------\n')
  log.write(' ENVIRONMENT BEGIN\n')
  log.write('---------------------------------------------\n')

  padding = max([len(key) for key in os.environ.keys()])
  format = '%' + str(padding) + 's %s\n'
  for param in sorted(os.environ.keys()):
    log.write(format % (param, os.environ[param]))

  log.write('---------------------------------------------\n')
  log.write(' ENVIRONMENT END\n')
  log.write('---------------------------------------------\n')

def create_target_specific_build_artifacts():
  artifacts = [ 'RPM_AAAAANAAR.mbn',
                'RPM_AAAAANAAR.elf',
                'RPM_AAAAANAAR.map',
                'RPM_AAAAANAAR.htm',
                'RPM_AAAAANAAR.scl',
                'RPM_AAAAANAAR.sym',
                'RPM_AAAAANAAR.hash',
                'RPM_AAAAANAAR_hash.hd',
                'RPM_AAAAANAAR_combined_hash.mbn',
                'RPM_AAAAANAAR_phdr.pbn',
                'RPM_AAAAANAAR_rpm.elf',
                'RPM_AAAAANAAR_scl.pp' ]
  clocktestdict_target = '../core/power/rpm/debug/scripts/ClockTestDict.py'
  clocktestdict_source = '../core/systemdrivers/clock/scripts/' + os.environ['CHIPSET'] + '/ClockTestDict.py'
                
  if os.environ['MSM_ID'] in ['8909','8909W']:
    target_artifact = '../core/bsp/rpm/build/'+os.environ['MSM_ID']+'/'+ os.environ['CHIPSET_PMIC']+'/'
  else:
    target_artifact = '../core/bsp/rpm/build/'+os.environ['MSM_ID']+'/'
	
  if os.path.exists('../core/bsp/rpm/build/RPM_AAAAANAAR.mbn'):
    print 'Copying build artifacts to '+target_artifact
    if not os.path.exists(target_artifact):
      os.makedirs(target_artifact)
    for artifact in artifacts:
      shutil.copy2('../core/bsp/rpm/build/'+artifact, target_artifact+artifact)

    if ( not os.path.exists(clocktestdict_target) ) and os.path.exists(clocktestdict_source):
      shutil.copy(clocktestdict_source, clocktestdict_target)
  else:
    if os.path.exists(clocktestdict_target):
      # make sure the file is writable; otherwise, an IOError exception will be raised.
      os.chmod(clocktestdict_target, stat.S_IWRITE)
      os.remove(clocktestdict_target)

    if os.path.exists(target_artifact):
      for artifact in artifacts:
        os.remove(target_artifact+artifact)
      os.rmdir(target_artifact)

def start_build():
  # the list of build arguments we need to pass to scons--note that we're
  # not actually setting any of these here--they should be set externally
  os.environ["SECBUILD"] = 'False' 
  
  sec_policy = []
  for arg in sys.argv:
    if arg == 'DISABLE_SEC_POLICY':
      if os.environ.has_key("SECPOLICY"):
        del os.environ["SECPOLICY"]
      os.environ["SECBUILD"] = 'False'
      sec_policy.append(arg)

  if len(sec_policy) > 0:
    os.environ["SECPOLICY"] = " ".join(sec_policy)

  if os.environ["SECBUILD"] == 'True' or os.environ.has_key("SECPOLICY"):
    print "Generating signed image"
  else:
    print "!! NOT GENERATING SIGNED IMAGE !!"

  if os.environ['MSM_ID'] in ['8909','8909W']:
     build_args = [
      'BUILD_ID',
      'BUILD_VER',
      'MSM_ID',
      'HAL_PLATFORM',
      'TARGET_FAMILY',
      'BUILD_ASIC',
      'CHIPSET',
      'SECBUILD',
      'CHIPSET_PMIC',
    ]
  else:
     build_args = [
      'BUILD_ID',
      'BUILD_VER',
      'MSM_ID',
      'HAL_PLATFORM',
      'TARGET_FAMILY',
      'BUILD_ASIC',
      'CHIPSET',
      'SECBUILD',
    ]
  
  if os.environ.has_key("SECPOLICY"):
    build_args.append("SECPOLICY")

  std_args    = ' '.join(['%s=%s' % (key, os.environ[key]) for key in build_args])
  passed_args ='' 
  for arg in sys.argv[1:]:
    if arg != 'build_common.py' and \
       arg != 'DISABLE_SEC_POLICY' and arg.find("USES_SEC_POLICY") != 0:	  
      passed_args += ''.join('%s ' %arg)
  print "CWD: "+os.getcwd()+"\n"
  if platform.system() == "Linux":
    build_cmd   = '../tools/build/scons/SCons/scons.sh -f ../tools/build/scons/build/start.scons %s %s' % (std_args, passed_args)
    #build_cmd   = '../tools/build/scons/SCons/scons.sh'
  else:
    build_cmd   = '..\\tools\\build\\scons\\SCons\\scons.bat -f ..\\tools\\build\\scons\\build\\start.scons %s %s' % (std_args, passed_args)

  print 'Setting up build environment...'
  os.environ['SCONS_BUILD_ROOT'] = '..'
  os.environ['SCONS_TARGET_ROOT'] = '../..'
  os.environ['TOOLS_SCONS_ROOT'] = '../tools/build/scons'

  msm_id = os.environ['MSM_ID']
  if os.path.exists('build-log_%s.txt' % (msm_id)):
    print 'Renaming the previous build log...'
    index = 1
    while os.path.exists('build-log_%s-%i.txt' % (msm_id, index)):
      index += 1
    shutil.move('build-log_%s.txt' % msm_id, 'build-log_%s-%i.txt' % (msm_id, index))
    
  buildlog = 'build-log_' + msm_id + '.txt'	
  log = open(buildlog, 'w')
  dump_env(log)

  print 'Executing build command:\n\t' + build_cmd
  the_build = subprocess.Popen(build_cmd, stderr=subprocess.STDOUT, stdout=subprocess.PIPE, shell=True)
  c = the_build.stdout.read(1)
  while c:
     log.write(c)
     sys.stdout.write(c)
     c = the_build.stdout.read(1)
  exit_code = the_build.wait()

  create_target_specific_build_artifacts()
  subprocess.Popen.wait(the_build)
  sys.exit(exit_code)

def print_build_time(start):
  stop = datetime.datetime.now()
  print '\n\n'
  print 'Build started:', str(start)
  print 'Build ended:  ', str(stop)
  print 'Elapsed time: ', str(stop - start)

if __name__ == '__main__':
  atexit.register(print_build_time, datetime.datetime.now())

  os.environ['BUILD_ID']  = os.environ.get('BUILD_ID', 'AAAAANAAR')
  os.environ['BUILD_VER'] = os.environ.get('BLD_ENV_VER_NUM_COMPRESSED', '0')

  # if ARMPATH is not defined then auto-detect the compiler.
  #   otherwise assume that all the env variables are already set.
  if( os.environ.get('ARMPATH', 'none') == 'none' ):
    if find_compiler():
      start_build()
  else:
    write_setenv_files_from_environment()
    start_build()

