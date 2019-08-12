#===============================================================================
#
# HEX file builder
#
# GENERAL DESCRIPTION
#    build rules script
#
# Copyright (c) 2009 - 2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/bsp/tools/flash/build/hostdl_gen_hex.py#1 $
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
# 05/07/10   sb       MBN builder changes for secure/nonsecure build
# 09/16/09   jz       Use objcopy to generate the final nandprg.hex
# 08/12/09   mh       Branch and update for supporting linux builds
#===============================================================================
import os
import subprocess
import string
import re

#------------------------------------------------------------------------------
# Global values
#------------------------------------------------------------------------------
HOSTDL_GEN = "objcopy"

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('hostdl_gen_hex_builder')

def generate(env):
   hostdl_hex_generate(env)

#===============================================================================
# PBN build rules
#===============================================================================
def hostdl_hex_generate(env):
   #-------------------------------------------------------------------------------
   # def builder for PBN
   #
   hosdl_hex_act = env.GetBuilderAction(hostdl_hex_builder)
   hosdl_hex_bld = env.Builder(action = hosdl_hex_act,
                               emitter = hostdl_hex_emitter,
                               suffix = '.hex')

   env.Append(BUILDERS = {'NPRGHexBuilder' : hosdl_hex_bld})

#-------------------------------------------------------------------------------
# Emitter builder to add clean actions
#
def hostdl_hex_emitter(target, source, env):

   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)
   return (target, source)

#-------------------------------------------------------------------------------
# def builder for qcoreapp_booting.pbn
#
def hostdl_hex_builder(target, source, env):
   # init and fix variebles
   source_str = str(source[0])
   target_str = str(target[0])

   #import pdb; pdb.set_trace()
   if (os.path.getsize(source_str) != 0):
      cmds = env.subst(HOSTDL_GEN +" -O ihex -I binary "+ source_str +" "+ target_str +"  --change-addresses ${HOSTDL_ADDR}")
      data, err, rv = env.RunCmds(target, source, cmds)
   else:
      open(target_str,"wb")		
   
   return None