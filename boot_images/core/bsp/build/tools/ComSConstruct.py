#===============================================================================
#
# ComSConstruct
#
# GENERAL DESCRIPTION
#    Common SConstruct script, it is intended to be used as a plug in from other
#    build locations. 
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/bsp/build/tools/ComSConstruct.py#1 $
#  $DateTime: 2015/09/01 00:30:35 $
#  $Author: pwbldsvc $
#  $Change: 8935858 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 12/18/09   wd      Created...
#
#===============================================================================
import os
import glob
import SCons
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('sconstruct')

def generate(env):
   #---------------------------------------------------------------------------
   # Set global variables and load defaults values and scripts
   #---------------------------------------------------------------------------
   env.Replace(PRINT_BUILD_BANNER = 'yes')
   env.Replace(ROOT_GLOBAL_ENV = env)
   env.Tool('defaults', toolpath = ["${BUILD_ROOT}/core/bsp/build/scripts"])

   #---------------------------------------------------------------------------
   # Load TARGET IMAGE scripts
   #  The images scripts should be located in 
   #  build_root/<AU name>/bps/<image name>/build/Sconscript
   #---------------------------------------------------------------------------
   #import pdb; pdb.set_trace()
   env.LoadImageUnits(env.get('IMAGE_ROOT_DIR_PATTERNS'))

   print "\n"
   print "------------------------------------------------------------------------------"

   # set default target if none is given, print actual targets
   env.Default('all')
   print "COMMAND_LINE_TARGETS are", map(str, SCons.Script.COMMAND_LINE_TARGETS)
   print "BUILD_TARGETS are       ", map(str, BUILD_TARGETS)

   print "\n"
   print "=============================================================================="

   # check for command line uknown variables
   vars = Variables()
   unknown_var = vars.UnknownVariables()
   if unknown_var:
      print "Unknown variables:", unknown_var.keys()
      Exit(1)
