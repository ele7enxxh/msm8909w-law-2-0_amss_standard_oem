#===============================================================================
#                    Copyright 2009 Qualcomm Technologies Incorporated.
#                           All Rights Reserved.
#                         QUALCOMM Proprietary/GTDR
#===============================================================================
import os
import fnmatch
import sys

def build_flags(env):    
   cmd = "python " + env.subst('$QURT_TOP') +"/kernel/scripts/build_flags.py " + env.subst('$CONFIG_DIR')
   return os.popen(cmd).read().replace('\n','')


def set_tools(env, tool_ver):
   if os.name == 'posix':
      env['Q6_TOOLS_ROOT'] = '/pkg/qct/software/hexagon/releases/tools/' + tool_ver
   elif os.name == 'nt':
      env['Q6_TOOLS_ROOT'] = 'C:/Qualcomm/HEXAGON_Tools/' + tool_ver
   else:
      print "Unknown platform!"
      sys.exit(0)     

def set_default_v4_env(env):
   env['Q6VERSION'] = 'v4'
   env['BUILD_CONFIG_FLAGS'] = env.BuildFlags()
   if env['Q6_TOOLS_ROOT'] == None:
      env.SetTools('3.1.04')

def set_default_v5_env(env):
   env['Q6VERSION'] = 'v5'
   env['BUILD_CONFIG_FLAGS'] = env.BuildFlags()
   if env['Q6_TOOLS_ROOT'] == None:
      env.SetTools('5.0.01')

def modemFWv4(env):
   env['CONFIG_DIR'] = env.subst('$QURT_TOP') + "/config/modemFWv4"
   env.SetDefaultV4Env()
   
def modemSWv4(env):
   env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/modemSWv4'
   env.SetDefaultV4Env()
   
def modemv5(env):
   env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/modemv5'
   env.SetDefaultV5Env()
   
def MMv4(env):
   env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/MMv4'
   env.SetDefaultV4Env()

def MMv5(env):
   env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/MMv5'  
   env.SetDefaultV5Env()

def ADSPv5MP(env):
   env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/ADSPv5MP'  
   env.SetDefaultV5Env()

def simv4(env):
   env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/sim'   
   env.SetDefaultV4Env()

def simv5(env):
   env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/sim'   
   env.SetDefaultV5Env()

def simv4_bitmask(env):
   env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/sim_bitmask'   
   env.SetDefaultV4Env()

def simv5_bitmask(env):
   env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/sim_bitmask'   
   env.SetDefaultV5Env()

def mpqurt(env):
   env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/mpqurt'   
   env.SetDefaultV5Env()

def staticv4(env):
   env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/static'   
   env.SetDefaultV4Env()

def staticv5(env):
   env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/static'   
   env.SetDefaultV5Env()

def MBAv5(env):
   env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/MBAv5'   
   env.SetDefaultV5Env()

target_functions = { 'modemFWv4': modemFWv4,
                     'modemSWv4': modemSWv4,
                     'modemv5'  : modemv5,
                     'MMv4'     : MMv4,
                     'MMv5'     : MMv5,
                     'ADSPv5MP'     : ADSPv5MP,
                     'simv4'    : simv4,
                     'simv5'    : simv5,
                     'simv4_bitmask': simv4_bitmask,
                     'simv5_bitmask': simv5_bitmask,
                     'mpqurt'   : mpqurt,
                     'staticv4' : staticv4,
                     'staticv5' : staticv5,
                     'MBAv5' : MBAv5 }
                    

def init_target(env):
  
  target=env.subst('$TRGT')
  if target=='modemFWv4':
     env.modemFWv4()
  elif target=='modemSWv4':
     env.modemSWv4()
  elif target=='modemv5':
     env.modemv5()
  elif target=='MMv4':
     env.MMv4()
  elif target=='MMv5':
     env.MMv5()
  elif target=='ADSPv5MP':
     env.ADSPv5MP()
  elif target=='simv4':
     env.simv4()
  elif target=='simv5':
     env.simv5()
  elif target=='simv4_bitmask':
     env.simv4_bitmask()
  elif target=='simv5_bitmask':
     env.simv5_bitmask()
  elif target=='mpqurt':
     env.mpqurt()
  elif target=='staticv4':
     env.staticv4()
  elif target=='staticv5':
     env.staticv5()
  elif target=='MBAv5':
     env.MBAv5()
  else:
     print "Unknwon TARGET!!!"
     sys.exit(0)

    


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
   
   #InitTarget method
   env.AddMethod(init_target, "InitTarget")
  
   #Setting up target initialization methods
   for target in target_functions:
      env.AddMethod(target_functions[target], target)
   env.AddMethod(build_flags, "BuildFlags")
   env.AddMethod(set_tools, "SetTools")
   
   # setup default environments
   env.AddMethod(set_default_v4_env, "SetDefaultV4Env")
   env.AddMethod(set_default_v5_env, "SetDefaultV5Env")

   #Init Generic Methods
   env.AddMethod(find_files, "FindFiles")
   env.AddMethod(install_dir, "InstallDir")

   #Hacks
   env.AddMethod(set_hacks, "SetHacks")
