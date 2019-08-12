#===============================================================================
#                    Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
#                         QUALCOMM Proprietary/GTDR
#===============================================================================
import os
import fnmatch
import sys  
import tempfile
import shutil
import cStringIO
import subprocess

#===============================================================================
# Find Files
#
#===============================================================================
def find_files (env, file_name_patterns='*', root_dir='.', posix=False, sort=True):
   # initialize variables
   ospath_join = os.path.join
   env_subst = env.subst
   aggregate_file_list = []
   
   # expand any SCons variables
   root_dir = env_subst(root_dir)   
   
   # make sure patterns is a list
   if type(file_name_patterns) is not list:
      file_name_patterns = [file_name_patterns]

   # walk trough each directory and try each pattern, aggregate results.
   for dirpath, dirs, files in os.walk(root_dir):
   #for files in os.walk(root_dir):
   
      for file_name_pattern in file_name_patterns:
   
         file_name_pattern = env_subst(file_name_pattern)

         files_match = fnmatch.filter(files, file_name_pattern)

         match_file_list = [ospath_join(dirpath, file) for file in files_match]
         match_file_list = [os.path.realpath(match_file) for match_file in match_file_list]
         aggregate_file_list.extend(match_file_list)
   
   # remove any duplicate matches from aggregate list.
   aggregate_file_list = list(set(aggregate_file_list))

   if posix:
      aggregate_file_list = [file_item.replace('\\', '/') for file_item in aggregate_file_list]
   
   if sort:
      aggregate_file_list = sorted(aggregate_file_list)

   return aggregate_file_list


#===============================================================================
# Install Directory method
# Wrapper api for Install which works for directories
#===============================================================================
def install_dir (env, target, source, file_pattern="*"):
 
   source = os.path.realpath(source)
   files = env.FindFiles(file_pattern,source)
   install_list = []
   
   for file in files:
      dir_part = file.replace(source,'')
      install_list += [env.Install(os.path.dirname(target+dir_part), file)]

   return install_list
   
#===============================================================================
# Set Hacks
# 
#===============================================================================
def set_hacks (env):
 
   env['ASCOM'] = env['ASPPCOM']

#Initialize environmentn 
def Init(env):

   #Init Generic Methods
   env.AddMethod(find_files, "FindFiles")
   env.AddMethod(install_dir, "InstallDir")
   env.AddMethod(RecursiveInstall)

   #Hacks
   env.AddMethod(set_hacks, "SetHacks")
   include_checker_generate(env)

#------------------------------------------------------------------------------
# include file checker
#------------------------------------------------------------------------------
def include_checker_generate(env):
   if env['EXTRA_WARNINGS']:
      import tempfile
      import shutil
      import cStringIO
      def include_check_builder(target, source, env):
         tdir = tempfile.mkdtemp(dir=os.path.dirname(str(target[0])))
         name_src = tdir+'/'+os.path.basename(str(source[0]))
         bname_h = env['INCLIST'][0]
         bname_target = 'ICGOT_' + bname_h.replace('.','_')
         name_h = tdir+'/'+bname_h
         shutil.copy(str(source[0]), name_src)
         f_inc = open(name_h, 'w')
         shutil.copyfileobj(cStringIO.StringIO('%s\n' % bname_target), f_inc)
         f_inc.close()
         envlocal = env.Clone(_CPPINCFLAGS=['-I'+tdir])
         cmd = envlocal.subst('$CC -E -nostdinc $CFLAGS $CCFLAGS $_CCCOMCOM')+' '+name_src
         try:
            cpp = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
         except:
            return 0
         not_found = True
         for line in cpp.stdout:
            if line.find(bname_target) >= 0:
               not_found = False
         cpp.stdout.close()
         cpp.wait()
         os.remove(name_src)
         os.remove(name_h)
         os.rmdir(tdir)
         touch_output = True
         if not_found:
            print "INFO:", str(source[0]), "does not include", bname_h
            touch_output = False
         if cpp.returncode != 0:
            print "INFO:", str(source[0]), "may contain include files other than", bname_h
            touch_output = False
         if touch_output:
            f_out = open(str(target[0]), 'w')
            shutil.copyfileobj(cStringIO.StringIO('File OK\n'), f_out)
            f_out.close()
         return 0
      icbuild = env.Builder(action = include_check_builder, suffix = '.chk', INCLIST=[])
      icbuild.action.cmdstr = None
      env['BUILDERS']['IncludeCheck'] = icbuild
   else:
      def no_include_checker(filename, **discard):
         return []
      env.IncludeCheck = no_include_checker

def recursive_install(env, path ):
    nodes = env.Glob \
        ( os.path.join(path, '*')
        , strings=False
        )
    nodes.extend \
            ( env.Glob \
                ( os.path.join(path, '*.*')
                , strings=False
                )
            )
    out = []
    for n in nodes:
        if n.isdir():
            out.extend( recursive_install(env, n.abspath ))
        else:
            out.append(n)

    return out

def RecursiveInstall(env, target, dir):
    nodes = recursive_install(env, dir)

    dir = env.Dir(dir).abspath
    target = env.Dir(target).abspath

    l = len(dir) + 1

    relnodes = [ n.abspath[l:] for n in nodes ]

    retval = []

    for n in relnodes:
        t = os.path.join(target, n)
        retval.append(t)
        s = os.path.join(dir, n)
        env.InstallAs ( env.File(t), env.File(s))
    return retval

