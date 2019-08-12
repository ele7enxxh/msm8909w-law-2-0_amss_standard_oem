#===============================================================================
#
# PIL Splitter builder
#
# GENERAL DESCRIPTION
#    Run the PIL Splitter tool on .mbn files.
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/build/scripts/pil_splitter_builder.py#1 $
#  $DateTime: 2016/06/17 14:31:11 $
#  $Author: pwbldsvc $
#  $Change: 10727476 $
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
from SCons.Script import *


#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('pil_splitter_builder')

def generate(env):
   splitter_generate(env)

#===============================================================================
# PBN build rules
#===============================================================================
def splitter_generate(env):
   #-------------------------------------------------------------------------------
   # def builder for PBN
   #
   splitter_act = env.GetBuilderAction(splitter_builder)
   splitter_bld = env.Builder(action  = splitter_act,
                              emitter = splitter_emitter)

   env.Append(BUILDERS = {'PilSplitterBuilder' : splitter_bld})

#-------------------------------------------------------------------------------
# Emitter builder to add clean actions
#
def splitter_emitter(target, source, env):
   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)

   # The PIL Splitter creates multiple files, and we have no way of knowing at this
   # point how many files there will be, so we can't explicitly add these files
   # to the target list at this time.
   target_str = os.path.abspath(str(target[0]))
   target_prefix = os.path.splitext(target_str)[0]
   target_flist_fn = target_prefix + '.flist'

   # Add targets
   #target.append(target_flist_fn)
   if os.path.exists(target_flist_fn):
      target_flist = open(target_flist_fn, 'r')
      file_list = target_flist.readlines()
      for file in file_list:
         target.append(os.path.join(env.subst("${BUILD_ROOT}"), file.strip()))
      env.Clean(target, target_flist_fn)

   return (target, source)

#-------------------------------------------------------------------------------
# def builder for qcoreapp_booting.pbn
#
def splitter_builder(target, source, env):
   # init and fix variebles
   source_str = os.path.abspath(str(source[0]))
   target_str = os.path.abspath(str(target[0]))
   target_prefix = os.path.splitext(target_str)[0]
   target_flist_fn = target_prefix + '.flist'

   cmds = ["python ${BUILD_ROOT}/core/securemsm/trustzone/qsee/build/scripts/pil-splitter.py %s %s" % (source_str, target_prefix)]

   data, err, rv = env.RunCmds(target, source, cmds)

   # Create a list of files that were created, so that we can clean them
   # later, or detect if one has been removed.
   target_flist = open(target_flist_fn, 'w')
   file_list = env.GlobFiles(target_prefix + ".b*")
   for file in file_list:
      rel_path = env.RelPath(file, env.subst("${BUILD_ROOT}")) + '\n'
      rel_path = rel_path.replace('\\', '/')
      target_flist.write(rel_path)
   target_flist.close()

   return None
