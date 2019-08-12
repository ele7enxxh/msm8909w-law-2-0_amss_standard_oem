import os, sys, subprocess, logging, re, shutil, stat, atexit, platform
from datetime import datetime

def show_runtime(start):
  end = datetime.now()
  #seconds = (end-start).total_seconds()
  #minutes = int(seconds / 60)
  #seconds -= 60 * minutes
  #print 'Time taken: %sm %ss' % (minutes, seconds)

def describe_func(func_description):
  def decorator(func):
    func.func_description = func_description
    def wrapper(*args, **kwargs):
      print '[' + datetime.now().strftime('%H:%M:%S') + ']',
      print '%s...' % func.func_description,
      result = func(*args, **kwargs)
      if not result:
        print 'FAILED'
        sys.exit(1)
      elif type(result) == bool:
        print 'ok'
      else:
        print result
      return result
    return wrapper
  return decorator

def trace_command(command):
  log = open('packrat.log', 'a')
  log.write('\n******************************\n')
  log.write('*** Running command: %s\n' % command)
  log.write('******************************\n')
  cmd_pipe = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
  stdoutdata, stderrdata = cmd_pipe.communicate()
  log.write('\nSTDOUT:\n\n' + stdoutdata)
  log.write('\nSTDERR:\n\n' + stderrdata)
  log.close()
  return stdoutdata, stderrdata

def delete_file(file):
  # ensure the read-only flag is not set on the file before deleting, or it will fail
  os.chmod(file, stat.S_IWUSR | stat.S_IRUSR | stat.S_IXUSR)
  os.remove(file)

def chomp(x):
    if x[-2:]=='\r\n': return x[:-2]
    if x[-1:]=='\n': return x[:-1]
    return x

def normalize(path):
  return os.path.normcase(os.path.normpath(chomp(path)))

@describe_func('Check that we\'re running from the base of an RPM build')
def running_from_rpm_build():
  print "\nCWD: " + ( re.escape(normalize(os.getcwd())) )
  return os.path.exists('build/build_common.py')

@describe_func('Identifying the MSM that this build targets')
def identify_msm_target(argv):
  if len(argv) < 2:
    return False
  if platform.system() == "Linux":
    build_script = 'build/build_%s.sh' % argv[1]
  else:
    build_script = 'build/build_%s.bat' % argv[1]
  if os.path.exists(build_script):
    return argv[1]
  return False

@describe_func('Clean the build of any known build targets')
def clean_build(msm):
  os.chdir('build')
  if platform.system() == "Linux":
    trace_command('./build_%s.sh -c' % msm)
  else:
    trace_command('.\\build_%s.bat -c' % msm)
  os.chdir('..')
  return True

@describe_func('Configure build for USES_NO_STRIP_NO_ODM + USES_NO_DEBUG to generate binary libraries, remove USES_INTERNAL_BUILD to strip features')
def configure_no_strip_no_debug(msm):
  msm_path = 'core/bsp/rpm/build'
  shutil.move(msm_path + '/rpm.scons', msm_path + '/rpm.scons.orig')
  orig = open(msm_path + '/rpm.scons.orig', 'r')
  new = open(msm_path + '/rpm.scons', 'w')

  for line in orig.readlines():
    if 'USES_NO_STRIP_NO_ODM' in line:
      line = line.lstrip('#')
    if 'USES_NO_DEBUG' in line:
      line = line.lstrip('#')
    if 'USES_INTERNAL_BUILD' in line:
      line = ""
    new.write(line)

  orig.close()
  new.close()
  delete_file(msm_path + '/rpm.scons.orig')
  return True

@describe_func('Build the RPM image')
def build(msm):
  os.chdir('build')
  if platform.system() == "Linux":
    trace_command('./build_%s.sh %s' % (msm, ' '.join(sys.argv[2:])))
  else:
    trace_command('.\\build_%s.bat %s' % (msm, ' '.join(sys.argv[2:])))
  os.chdir('..')
  return True

@describe_func('Configure build to the customer release configuration')
def configure_customer_release(msm):
  msm_path = 'core/bsp/rpm/build'
  shutil.move(msm_path + '/rpm.scons', msm_path + '/rpm.scons.orig')
  orig = open(msm_path + '/rpm.scons.orig', 'r')
  new = open(msm_path + '/rpm.scons', 'w')

  for line in orig.readlines():
    if 'USES_NO_STRIP_NO_ODM' in line:
      line = '#' + line.lstrip('#')
    if 'USES_NO_DEBUG' in line:
      line = '#' + line.lstrip('#')
    if 'USES_INTERNAL_BUILD' in line:
      line = ""
    new.write(line)

  orig.close()
  new.close()
  delete_file(msm_path + '/rpm.scons.orig')
  return True

@describe_func('Check that the build was successful')
def check_elf_exists(msm):
  msm_path = 'build/ms/bin/' + msm
  if msm == '8909':
    return ((os.path.exists(msm_path + '/' + 'pm8909' + '/rpm.mbn')) and (os.path.exists(msm_path + '/' + 'pm8916' + '/rpm.mbn')))
	
  return os.path.exists(msm_path + '/rpm.mbn')

@describe_func('Create local scons.bat that runs the build in the python profiler')
def create_profiled_scons():
  if platform.system() == "Linux":
    shutil.move('tools/build/scons/SCons/scons.sh', 'tools/build/scons/SCons/scons.sh.normal')
    shutil.move('tools/build/scons/SCons/profiled_scons.sh', 'tools/build/scons/SCons/scons.sh')
  else:
    shutil.move('tools/build/scons/SCons/scons.bat', 'tools/build/scons/SCons/scons.bat.normal')
    shutil.move('tools/build/scons/SCons/profiled_scons.bat', 'tools/build/scons/SCons/scons.bat')
  return True

@describe_func('Restore the normal scons.bat')
def restore_regular_scons():
  if platform.system() == "Linux":
    shutil.move('tools/build/scons/SCons/scons.sh', 'tools/build/scons/SCons/profiled_scons.sh')
    shutil.move('tools/build/scons/SCons/scons.sh.normal', 'tools/build/scons/SCons/scons.sh')
  else:
    shutil.move('tools/build/scons/SCons/scons.bat', 'tools/build/scons/SCons/profiled_scons.bat')
    shutil.move('tools/build/scons/SCons/scons.bat.normal', 'tools/build/scons/SCons/scons.bat')
  return True

@describe_func('Run a simulated build, collecting scons dependency information')
def build_with_nobuild_and_tree_output(msm):
  os.chdir('build')
  if platform.system() == "Linux":
    trace_command('./build_%s.sh -n --tree=all,prune' % msm)
  else:
    trace_command('.\\build_%s.bat -n --tree=all,prune' % msm)
  os.chdir('..')
  return True

flist = set()
	
@describe_func('Parse dependency/profiling information to generate a list of all files used in build')
def parse_files_used(file_list,msm_id):
  # Figure out which SConscripts are used
  os.chdir('build')
  stats, statserr = trace_command('python -c "import pstats; pstats.Stats(\'buildstats\').print_stats()"')
  os.chdir('..')
  cwd = re.escape(normalize(os.getcwd()))
  stats = normalize(stats)
  profiler_used_files = re.findall(cwd + '[^:]+', stats)
  file_list.update(set(profiler_used_files))


  # Figure out what files scons uses in a build
  logfile = 'build-log_' + msm_id + '.txt'	
  # Figure out what files scons uses in a build
  log_location = 'build/' + logfile
  build_log = open(log_location, 'r')
  
  scons_used_files = []
  for line in build_log.readlines():
    m = re.search('\+-(?P<file>.+)', normalize(line))
    if m:
      scons_used_files.append(m.group('file'))
  file_list.update(set(scons_used_files))

  
  if msm_id == '8909':
    file_location = 'build/build-log_' + msm_id + '.txt'	
    if os.path.exists(file_location):
        index = 1
        while os.path.exists('build/build-log_%s-%i.txt' % (msm_id, index)):
          index += 1
        logfile = 'build-log_%s-%i.txt' % (msm_id, index-1)
        log_location = 'build/' + logfile
        build_log = open(log_location, 'r')
        scons_used_files = []
        for line in build_log.readlines():
          m = re.search('\+-(?P<file>.+)', normalize(line))
          if m:
            scons_used_files.append(m.group('file'))
        
        flist.update(set(scons_used_files))
  
  return 'found %s used files' % len(file_list)

@describe_func('Collect a list of all files in the build tree')
def list_all_files(file_list):
  for root, dirs, files in os.walk('.'):
    root = os.path.abspath(root)
    file_list.update(set([normalize(os.path.join(root, f)) for f in files]))
  return 'found %s files total' % len(file_list)

@describe_func('Selecting additional whitelisted files as used')
def match_whitelist(all_files, whitelist_files, msm):
  packrat_path = os.path.abspath(os.path.dirname(sys.argv[0]))
  whitelist = {'MSM_ID':msm}
  if msm == '8909':
    execfile(packrat_path + '/whitelist_8909.py', whitelist)
  else:
    execfile(packrat_path + '/whitelist.py', whitelist)
  matches = [f for rule in whitelist['rules'] for f in all_files if re.search(rule, f)]
  log = open('./build/whitelist_log.txt', 'a')
  for match in matches:
    log.write(match+'\n')
  log.close()
  whitelist_files.update(matches)
  return 'matched %s' % len(matches)

@describe_func('Remove files that do not contribute to the build')
def remove_unused_files(file_list):
  for file in file_list:
    delete_file(file)
  return 'removed %s' % len(file_list)

@describe_func('Clean up any empty directories')
def clean_empty_directories():
  # clean up empty directories -- note, rmdir will refuse non-empty, hence
  # the lack of checking for that and the try catch block
  empty_dir_count = 0
  for root, dirs, files in os.walk('.', topdown=False):
    for name in dirs:
      try:
        dirname = os.path.join(root, name)
        os.chmod(dirname, stat.S_IWUSR | stat.S_IRUSR | stat.S_IXUSR)
        os.rmdir(dirname)
        empty_dir_count += 1
      except:
        pass
  return 'removed %s' % empty_dir_count

@describe_func('Clean up after packrat')
def final_cleanup():
  packrat_files = [
    'packrat.log',
    'build/packrat.log',
    'build/buildstats',
    'build/build.log',
    'build/buildstats',
  ]
  for f in packrat_files:
    if os.path.exists(f):
      delete_file(f)
  return True


def main(argv):
  start = datetime.now()
  atexit.register(show_runtime, (start))

  # housekeeping
  running_from_rpm_build()
  msm = identify_msm_target(argv)

  if msm == '8936': 
    supported_targets = ['8916','8936']
  else:
    supported_targets = [msm]
	
  for msm_id in supported_targets:
  # build the binary libraries
    configure_no_strip_no_debug(msm_id)
    build(msm_id)

  #ensure that the build works using the binary libraries
    configure_customer_release(msm_id)
    clean_build(msm_id)
    build(msm_id)
    check_elf_exists(msm_id)

  #profile and remove unused files
    clean_build(msm_id)
  
  
  used_files = set()
  whitelist_files = set()
  
  create_profiled_scons()
  
  for msm_id in supported_targets:
    build_with_nobuild_and_tree_output(msm_id)
    parse_files_used(used_files, msm_id)
  
  all_files = set()
  list_all_files(all_files)

  for msm_id in supported_targets:  
    match_whitelist(all_files, whitelist_files, msm_id)
  
  if msm_id == '8909':
    remove_unused_files(all_files - used_files - whitelist_files - flist)
  else:
    remove_unused_files(all_files - used_files - whitelist_files)
	
  clean_empty_directories()

  # finally, ensure the build still works
  restore_regular_scons()
  for msm_id in supported_targets:
    build(msm_id)
    check_elf_exists(msm_id)

if __name__ == '__main__':
  main(sys.argv)

