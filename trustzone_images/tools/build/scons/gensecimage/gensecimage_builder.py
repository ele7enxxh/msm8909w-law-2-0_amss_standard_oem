#===============================================================================
#
# MBN TOOLS
#
# GENERAL DESCRIPTION
#    Contains all MBN Utilities for image generation 
#
# Copyright 2010 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: 
#  $DateTime: 2016/06/17 14:31:11 $
#  $Author: pwbldsvc $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#
#===============================================================================
import os
import glob
import shutil
import SCons

GENSECIMAGE_DIR = os.path.dirname(os.path.realpath(__file__))

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('gensecimage_mbn_builder')

def generate(env):
   gensecimage_mbn_generate(env)
   
   gensecimage_cfg_generate(env)   

#===============================================================================
# PBN build rules
#===============================================================================
def gensecimage_mbn_generate(env):
   #-------------------------------------------------------------------------------
   # def builder for PBN
   #
   #import pdb;pdb.set_trace()
   env.AddMethod(gensecimage_builder_wrapper, "GensecimageMbnBuilder")
   env.AddMethod(gensecimage_builder_install, "GensecimageInstall")    
   
   gensecimage_mbn_act = env.GetBuilderAction(gensecimage_mbn_builder)
   gensecimage_mbn_bld = env.Builder(
						 action = gensecimage_mbn_act,
					     emitter = gensecimage_mbn_emitter,
                         suffix = '.mbn')

   env.Append(BUILDERS = {'GensecimageMbnInternalBuilder' : gensecimage_mbn_bld}) 
   
   os.environ['GENSECIMAGE_LOCK_ENABLE'] = 'false'

def gensecimage_cfg_generate(env):
   #-------------------------------------------------------------------------------
   # def builder for PBN
   #
   #import pdb;pdb.set_trace() 
   env.AddMethod(gensecimage_cfg_builder_wrapper, "GensecimageCfgBuilder")   
   
   gensecimage_cfg_act = env.GetBuilderAction(gensecimage_cfg_builder)
   gensecimage_cfg_bld = env.Builder(
						 action = gensecimage_cfg_act,
					     emitter = gensecimage_cfg_emitter,
                         suffix = '.cfg')

   env.Append(BUILDERS = {'GensecimageCfgInternalBuilder' : gensecimage_cfg_bld})
  
def gensecimage_builder_wrapper(env, 
                    target_base_dir,
                    source, 
                    enable_buildtags='USES_GENSECIMAGE', 
                    disable_buildtags=['USES_SECBOOT', 'USES_ENCRYPT_MBN'],
                    basic_cfg_template='gensecimage.cfg.template',
                    basic_cfg='gensecimage.cfg',
                    signing_cfg_template='signingattr_qpsa.cfg.template',
                    signing_cfg='signingattr_qpsa.cfg',
                    postsign_cfg_from_builder=None,
                    gensecimage_section=None, generate_signcfg=True, generate_cfg=True, 
                    gensecimage_msmid=None, gensecimage_jtagid=None,
                    gensecimage_enable_encrypt=False,
                    msm_jtag_mapping_file='${BUILD_ROOT}/build/msm_jtag_mapping.txt'):
   rt = None
   
   # don't do anything if none of the tags match.
   if (env.IsKeyEnable(enable_buildtags) is True) and (env.IsKeyEnable(disable_buildtags) is False):  
       if type(gensecimage_jtagid) is not list:
          gensecimage_jtagid = [gensecimage_jtagid]
       if type(gensecimage_msmid) is not list:
          gensecimage_msmid = [gensecimage_msmid]
       
       num_targets = max(len(gensecimage_msmid), len(gensecimage_jtagid))
       
       rt_list = []
       target_dir_realpath = env.RealPath(target_base_dir)
       target_dir_extended = None
       source_str = os.path.abspath(str(source[0]))
       source_path, source_file = os.path.split(source_str) 
       postsign_cfg_full = None
       for i in range(0, num_targets):
           #import pdb;pdb.set_trace() 
           if len(gensecimage_jtagid)>len(gensecimage_msmid):           
               target_dir_extended =  gensecimage_jtagid[i]
           else:
               target_dir_extended = gensecimage_msmid[i]
           
           target_path = os.path.join(target_dir_realpath, target_dir_extended)
           target_str = os.path.join(target_path, source_file)
           basic_cfg_full = os.path.join(target_path, basic_cfg)
           signing_cfg_full = os.path.join(target_path, signing_cfg)
           if postsign_cfg_from_builder is not None and i<len(postsign_cfg_from_builder):
               postsign_cfg_full = os.path.abspath(str(postsign_cfg_from_builder[i][0]))
           
           env.PrintDebugInfo('gensecimage', "gensecimage_builder_wrapper:target_str=%s" % target_str)
           
           #import pdb;pdb.set_trace()  
           if generate_cfg is True:
               if (gensecimage_enable_encrypt is True):
                   enable_encrypt = "yes"
               else:
                   enable_encrypt = "no"                    
                    
               rtt = env.GensecimageCfgInternalBuilder(target=basic_cfg_full, 
                    source=env.RealPath(basic_cfg_template),
                    GENSECIMAGE_SECTION=gensecimage_section,
                    GENSECIMAGE_CFG_TAGS_TO_REPLACE=[gensecimage_section.upper() + "_SRCFILE", gensecimage_section.upper() + "_OUTPUTDIR", "ENABLE_ENCRYPT"],
                    GENSECIMAGE_CFG_REPLACE_VALUES=[source_str, target_path, enable_encrypt])
               env.Depends(rt_list, rtt)
          
           if generate_signcfg is True:
               gensecimage_dir = env.get('GENSECIMAGE_DIR', GENSECIMAGE_DIR)
               env.PrintInfo("gensecimage_builder_wrapper: GENSECIMAGE_DIR = %s" % gensecimage_dir)  
               
               msm_jtag_mapping_file = env.subst(msm_jtag_mapping_file)               
               if not os.path.isfile(msm_jtag_mapping_file):
                   returnError(env, "gensecimage_builder_wrapper: msm to jtag mapping file %s does not exist!"  % msm_jtag_mapping_file)
               env.PrintInfo("gensecimage_builder_wrapper: msm_jtag_mapping_file= %s" % msm_jtag_mapping_file)
               
               jtag_id = getJTAGID(msm_jtag_mapping_file, gensecimage_jtagid, gensecimage_msmid, i)
                                    
               rtt = env.GensecimageCfgInternalBuilder(target=signing_cfg_full, 
                    source=env.RealPath(signing_cfg_template),
                    GENSECIMAGE_SECTION=gensecimage_section,
                    GENSECIMAGE_CFG_TAGS_TO_REPLACE=["JTAGID"],
                    GENSECIMAGE_CFG_REPLACE_VALUES=[jtag_id])                       
               env.Depends(rt_list, rtt)
           
           rtt = env.GensecimageMbnInternalBuilder(target_str, source, GENSECIMAGE_SECTION=gensecimage_section,
                BASIC_CFG=basic_cfg_full,
                SIGNING_CFG=signing_cfg_full,
                POSTSIGN_CFG=postsign_cfg_full)
           rt_list.append(rtt)
           
       return rt_list
   
   return rt
   
def gensecimage_builder_install(env, install_base_dir, install_sub_dir, install_file_name, gensecimage_builder_output=None,
                                alternate_source = None):
   rt_list = []
   
   if gensecimage_builder_output is not None:
       if type(install_sub_dir) is not list:
           install_sub_dir = [install_sub_dir]
          
       for i in range(0, len(install_sub_dir)):
           target_file_base, target_file_ext = os.path.splitext(install_file_name)
           target_path = os.path.join(install_base_dir, install_sub_dir[i])
           target_file_path = os.path.join(target_path, install_file_name)
           if alternate_source is not None:
               install_signed_mbn = env.InstallAs(target_file_path, alternate_source)
           else:
               install_signed_mbn = env.InstallAs(target_file_path, gensecimage_builder_output[i][0])
           #import pdb;pdb.set_trace() 
           install_signed_cert = env.InstallAs(os.path.join(target_path, 'cert', target_file_base + '.zip'), gensecimage_builder_output[i][1])
           rt_list.append([install_signed_mbn, install_signed_cert])
   
   return rt_list
   
#-------------------------------------------------------------------------------
# Emitter builder to add clean actions
#
def gensecimage_mbn_emitter(target, source, env):
   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)          
   target_str = os.path.abspath(str(target[0]))       

   target_path, target_file = os.path.split(target_str)
   signed_file = target_str
   signed_file_base, ext = os.path.splitext(signed_file)
   
   files_in_dir = glob.glob(target_path)
   for file in files_in_dir:
       if (file != signed_file):
           #SideEffect doesn't seem to clean the file here
           #env.SideEffect(file, target)   
           env.Clean(target, file)           

   #import pdb;pdb.set_trace()
   # Add this side effect to prevent concurrency access
   log_dir = env.subst('${BUILD_ROOT}/build/ms/')  
   env.SideEffect(os.path.join(log_dir, 'gensecimage.log'), target)
   env.PrintDebugInfo('gensecimage', "gensecimage sideeffect:%s" % os.path.join(log_dir, 'gensecimage.log'))                  
   
   gensecimage_cfg = env.get('BASIC_CFG', None)
   if gensecimage_cfg is not None:
       source.append(gensecimage_cfg)
   gensecimage_signcfg = env.get('SIGNING_CFG', None)
   if gensecimage_signcfg is not None:
       source.append(gensecimage_signcfg)
   gensecimage_postsigncfg = env.get('POSTSIGN_CFG', None)
   if gensecimage_postsigncfg is not None:
       source.append(gensecimage_postsigncfg)   
   
   target.append(signed_file_base + '.zip')
   
   return (target, source)

#-------------------------------------------------------------------------------
# def builder for qcoreapp_booting.pbn
#
def gensecimage_mbn_builder(target, source, env):
   # init and fix variebles      
   #import pdb;pdb.set_trace()
   source_str = os.path.abspath(str(source[0]))
   target_str = os.path.abspath(str(target[0]))
   target_path, target_file = os.path.split(target_str)
   
   gensecimage_dir = env.get('GENSECIMAGE_DIR', GENSECIMAGE_DIR) 
   env.PrintInfo("gensecimage_mbn_builder: GENSECIMAGE_DIR = %s" % gensecimage_dir)   
          
   # Get parameters from environment
   gensecimage_section = env.get('GENSECIMAGE_SECTION', None)
   if gensecimage_section is None:
       raise RuntimeError, "gensecimage_mbn_builder: GENSECIMAGE_SECTION cannot be None"               

   gensecimage_cfg = env.get('BASIC_CFG', None)            
   gensecimage_signcfg = env.get('SIGNING_CFG', None)  
   gensecimage_postsigncfg = env.get('POSTSIGN_CFG', None)       
       
   env.PrintInfo("gensecimage_mbn_builder: gensecimage config = %s" % gensecimage_cfg)
   env.PrintInfo("gensecimage_mbn_builder: signing config = %s" % gensecimage_signcfg)
   env.PrintInfo("gensecimage_mbn_builder: postsign config = %s" % gensecimage_postsigncfg)
   #import pdb;pdb.set_trace()   
                 
   # Issuing gensecimage command   
   cmds = env.subst("python " + os.path.join(gensecimage_dir, "gensecimage.py") + " --stage=qpsa:sign " + \
                    "--section=" + gensecimage_section.lower() + " " + \
                    "--config=" + gensecimage_cfg)
            
   if gensecimage_signcfg is not None:
       cmds = cmds + env.subst(" --signconfig=" + gensecimage_signcfg) 
            
   if gensecimage_postsigncfg is not None:
       cmds = cmds + env.subst(" --postsignconfig=" + gensecimage_postsigncfg)        
   
   data, err, rv = env.ExecCmds(cmds, target=target_str)
   
   # Backup gensecimage log file
   if os.path.exists('gensecimage.log'):
       shutil.move('gensecimage.log',  target_path)  

   # Remove qpsa log file as it is copied already
   if os.path.exists('qpsa.log'):
       os.remove('qpsa.log')   
    
   return None               
   
   
def gensecimage_cfg_builder_wrapper(env, 
                    target_base_dir,
                    target_filename,
                    source, 
                    tags_to_replace,
                    replace_values,
                    gensecimage_section,                    
                    enable_buildtags='USES_GENSECIMAGE', 
                    disable_buildtags=['USES_SECBOOT', 'USES_ENCRYPT_MBN'],
                    target_sub_dir=None,
                    ):
   rt = None
   
   # don't do anything if none of the tags match.
   if (env.IsKeyEnable(enable_buildtags) is True) and (env.IsKeyEnable(disable_buildtags) is False):  
       #import pdb;pdb.set_trace()
       if type(tags_to_replace) is not list:
          tags_to_replace = [tags_to_replace]
       if type(replace_values) is not list:
          replace_values = [replace_values]
       if type(target_sub_dir) is not list:
          target_sub_dir = [target_sub_dir]          
       
       num_targets = len(target_sub_dir)
       
       rt_list = []
       target_dir_realpath = env.RealPath(target_base_dir)   
       for i in range(0, num_targets):           
           target_path = os.path.join(target_dir_realpath, target_sub_dir[i])
           target_str = os.path.join(target_path, target_filename)
           
           env.PrintDebugInfo('gensecimage', "gensecimage_cfg_builder_wrapper:target_str=%s" % target_str)
           
           #import pdb;pdb.set_trace()  
           rt1 = env.GensecimageCfgInternalBuilder(target=target_str, 
                source=env.RealPath(source),
                GENSECIMAGE_SECTION=gensecimage_section,
                GENSECIMAGE_CFG_TAGS_TO_REPLACE=tags_to_replace, 
                GENSECIMAGE_CFG_REPLACE_VALUES=replace_values)
           rt_list.append(rt1)          
       
       return rt_list
   
   return rt
   
   
#-------------------------------------------------------------------------------
# Emitter builder to add clean actions
#
def gensecimage_cfg_emitter(target, source, env):
   # here one can also add dependencies for say aexe.exe (if it needs to be build)
    
   
   return (target, source)

#-------------------------------------------------------------------------------
# def builder to generate gensecimage.cfg from template
#
def gensecimage_cfg_builder(target, source, env):
   # init and fix variebles      
   
   source_str = os.path.abspath(str(source[0]))
   target_str = os.path.abspath(str(target[0]))

   # Get parameters from environment
   gensecimage_section = env.get('GENSECIMAGE_SECTION', None)
   if gensecimage_section is None:
       raise RuntimeError, "gensecimage_cfg_builder: GENSECIMAGE_SECTION cannot be None"       
   
   tag_list = env.get('GENSECIMAGE_CFG_TAGS_TO_REPLACE', None)
   if tag_list is None:
       raise RuntimeError, "gensecimage_cfg_builder: GENSECIMAGE_CFG_TAGS_TO_REPLACE cannot be None"
   if type(tag_list) is not list:
       tag_list = [tag_list]
   
   value_list = env.get('GENSECIMAGE_CFG_REPLACE_VALUES', None)
   if value_list is None:
       raise RuntimeError, "gensecimage_cfg_builder: GENSECIMAGE_CFG_REPLACE_VALUES cannot be None"
   if type(value_list) is not list:
       value_list = [value_list]       
   
   # Generate gensecimage config
   if not os.path.isfile(source_str):
       returnError(env, "gensecimage_cfg_builder: source_str=%s does not exist!"  % source_str)

   #import pdb;pdb.set_trace()
   generate_config(source_str, target_str, \
                gensecimage_section, 
                tag_list,
                value_list)
    
   return None   
   
# Use returnError() when the error is expected. 
# Use raise RuntimeError when the error is unexpected and call stack is needed.
def returnError(env, errorMsg):
   env.PrintError("")
   env.PrintError("-------------------------------------------------------------------------------")
   env.PrintError(errorMsg)
   env.PrintError("-------------------------------------------------------------------------------")
   env.PrintError("")
   SCons.Script.Exit(1)
    
def getJTAGID(msm_jtag_mapping_file, gensecimage_jtagid, gensecimage_msmid, i):
   if i<len(gensecimage_jtagid):
       gensecimage_jtagid_i = gensecimage_jtagid[i]
   else:
       gensecimage_jtagid_i = None
       
   if i<len(gensecimage_msmid):
       gensecimage_msmid_i = gensecimage_msmid[i]
   else:
       gensecimage_msmid_i = None
       
   # Read MSM-JTAGID mapping from file                 
   MSM_TO_JTAG_DICT = {}
   with open(msm_jtag_mapping_file) as f:
       for line in f:
          (key, val) = line.split()
          MSM_TO_JTAG_DICT[key] = val
   f.close()
   
   # Initialize jtag_id
   if gensecimage_jtagid_i is not None:
       jtag_id = gensecimage_jtagid_i
   elif gensecimage_msmid_i in MSM_TO_JTAG_DICT:
       jtag_id = MSM_TO_JTAG_DICT[gensecimage_msmid_i]
   else:
       raise RuntimeError, "gensecimage getJTAGID: JTAG ID is missing in generating sign config"

   return jtag_id       
    
def generate_config(src_config_file, dest_config_file, gensecimage_section, tag_list, value_list):
   if len(tag_list) != len(value_list):
       returnError(env, "gensecimage generate_config: length of tag_list (%d) is different from value_list(%d)" \
                % (len(tag_list), len(value_list)))
   
   new_file = open(dest_config_file,'w')
   old_file = open(src_config_file, 'r')
   for line in old_file:
       for i in range(0, len(tag_list)):
           #import pdb;pdb.set_trace()
           line = line.replace("%" + tag_list[i] + "%", value_list[i])           
       new_file.write(line)
   new_file.close()
   old_file.close() 