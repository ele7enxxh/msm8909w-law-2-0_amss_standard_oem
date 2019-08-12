#============================================================================
#
# Analytic Data Builders rules
#
# GENERAL DESCRIPTION
#    Plug-in like builder to collect build information for later processing 
#    and data mining for example lint, klocwork, etc.
#
# Copyright (c) 2009-2013 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/tools/build/scons/scripts/analytic_data_builder.py#1 $
#  $DateTime: 2015/09/01 00:30:35 $
#  $Author: pwbldsvc $
#  $Change: 8935858 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# YYYY-MM-DD   who     what, where, why
# ----------   ---     ---------------------------------------------------------
#
#
#============================================================================
import os
import re
import socket
import SCons
import uuid
import datetime
from xml.etree import ElementTree as et

#----------------------------------------------------------------------------
# Global values
# Global definitions used within this script for example tool cmd definitions
# which might be use in different places.
#----------------------------------------------------------------------------
TOOL_CMD = "${BUILD_ROOT}/tools/xyz/foo${EXE_EXT}"

"""============================================================================
Hooks for SCons
These are the function entry points SCons calls when loading this script.
============================================================================"""
def exists(env):
   '''
   Used by SCons to make sure scripts are not allowed to be
   loaded multiple times per environment.
   '''
   return env.Detect('version_builder')

def generate(env):
   '''
   This is the entry point called by SCons when loading this script.
   This should call other generate functions as a script might define
   multiple builders or methods.
   '''
   
   # -------------------------------------------------------------------------
   # Init environment for this group of builders
   # -------------------------------------------------------------------------
   
   # init image environment
   img_env = env.get('IMAGE_ENV')

   if img_env is None:
      env.PrintError('')
      env.PrintError("-------------------------------------------------------------------------------")
      env.PrintError("Analytic data builder, is being loaded too early or improperly. This builder") 
      env.PrintError("needs to be loaded within the context of a SCons image.")
      env.PrintError("-------------------------------------------------------------------------------")
      env.PrintError('')
      Exit(1)

   img_env.Replace(ANALYTIC_DATA = [])

   # init uses flags to show builder is loaded
   env.AddUsesFlags('USES_ANALYTIC_DATA')
   
   # register for CC, CXX, and AS actions
   env.PrependCCAction(comcc_data_capture, action_text='Analytics ')
   env.PrependCXXAction(comcxx_data_capture, action_text='Analytics ')
   env.PrependASAction(comas_data_capture, action_text='Analytics ')
   
   # init builders generates
   source_list_builder_generate(env)

"""============================================================================
Utility ACTION fucntions to intercept the CC, CXX, and AS actions
============================================================================"""
def com_data_capture(target, source, env, cmd):
   '''-------------------------------------------------------------------------
   com_data_capture

   common action fucntion to capture CC, CXX, or AS actiosn
   -------------------------------------------------------------------------'''

   target_str = str(target[0])
   source_str = str(source[0])
   #print "WD-src>>>:" + source_str
   #print "WD-cmd>>>:" + cmd
   
   img_env = env.get('IMAGE_ENV')

   if img_env is not None:
      analytic_data_array = img_env.get('ANALYTIC_DATA')
   
      if analytic_data_array is not None:
         analityc_data_node = dict()
         analityc_data_node['target'] = target_str         
         analityc_data_node['source'] = source_str
         analityc_data_node['cmd'] = cmd
         analytic_data_array.append(analityc_data_node)

def comcc_data_capture(target, source, env):
   '''-------------------------------------------------------------------------
   comcc_data_capture

   CC action fucntion to capture COMCC actiosn
   -------------------------------------------------------------------------'''
   cmd = env['CCCOMCPY']
   return com_data_capture(target, source, env, cmd)

def comcxx_data_capture(target, source, env):
   '''-------------------------------------------------------------------------
   comcxx_data_capture

   CXX action fucntion to capture COMCXX actiosn
   -------------------------------------------------------------------------'''
   cmd = env['CXXCOMCPY']
   return com_data_capture(target, source, env, cmd)
   
#------------------------------------------------------------------------------
# AS data capture function.
#------------------------------------------------------------------------------
def comas_data_capture(target, source, env):
   '''-------------------------------------------------------------------------
   comas_data_capture

   AS action fucntion to capture COMAS actiosn
   -------------------------------------------------------------------------'''
   cmd = env['ASCOMCPY']
   return com_data_capture(target, source, env, cmd)

"""============================================================================
Source List builder
============================================================================"""

def source_list_builder_generate(env):
   '''-------------------------------------------------------------------------
   Source list builder generate

   Register builder and methods with SCons.
   -------------------------------------------------------------------------'''

   # use standard action wrapper provided by QCT extensions, provides debug
   # levels and correct text output during build. Not using this will result
   # in function pointers and mangle names displayed instead of a nice string
   # such as "=== Generating  <your target>"
   source_list_act = env.GetBuilderAction(source_list_builder)


   source_list_bld = env.Builder(
      action = source_list_act,
      #emitter=source_list_emitter,
      #source_scanner=source_list_source_scanner,
      #target_scanner=source_list_target_scanner,
      #suffix = '.my_extention'
   )

   env.Append(BUILDERS = {'SourceListBuilder' : source_list_bld})

def source_list_builder(target, source, env):
   '''-------------------------------------------------------------------------
   Source list builder action

   The actual builder action this gets called only if the targets needs to
   be build or rebuild. Otherwise this will not be called. When done
   correctly (tell SCons your exact dependencies with scanners or emitters)
   this is a beautiful thing, it does the right thing when needed.
   -------------------------------------------------------------------------'''

   # targets and sources are ALWAYS arrays (is a builder standard interface)
   # this builder needs to know if it deals with multiple targes and sources
   # or singles, or mix (common use case: one target multiple sources).
   target_full = str(target[0])
   source_full = str(source[0])

   # suppose you need to call an the external tool use the following
   #cmd = "".join([TOOL_CMD, " -o=", target_full, " -i=", source_full])
   #std_out, std_err, return_val = env.ExecCmds(cmds)

   # another possibility for the builder is that does everything here in python
   # and then just use open file and write file to generate the target.
   # <fancy/magical python code here>
   
   sources_list = []
   img_env = env.get('IMAGE_ENV')
   
   if img_env is not None:
      analytic_data_array = img_env.get('ANALYTIC_DATA')
      
   sources_list = [analityc_data_node['source'] for analityc_data_node in analytic_data_array]

   env.ListFileCreate(target_full, sources_list, env.get('add_header', False), 
      rel_path_target=env.get('relative_path'), posix=env.get('posix', True), sort=env.get('sort', True))
   
def source_list_emitter(target, source, env):
   '''-------------------------------------------------------------------------
   QC Version builder emitter

   Modify a target dependencies on sources (see SCons documentation) and also
   a good place to add Clean rules, Dependency rules, Side effects, etc.
   -------------------------------------------------------------------------'''

   # a dependency to this builder such that if tool is missing builder will fail
   # also if such tool changes (newer tool) builder will be called to regenerate
   # target even if sources didn't change, since we are adding the tool as a
   # dependency.

   #env.Depends(target, env.RealPath(TOOL_CMD))

   return (target, source)

