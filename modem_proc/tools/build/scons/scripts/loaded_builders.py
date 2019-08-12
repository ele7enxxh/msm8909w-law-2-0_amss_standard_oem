'''
   Loaded Builders rules

  GENERAL DESCRIPTION
      Plug-in like builder to collect information on builders loaded for
      later processing and data mining for example lint, klocwork, etc.

'''

#============================================================================
#
# Loaded Builders rules
#
# GENERAL DESCRIPTION
#    Plug-in like builder to collect information on builders loaded for
#    later processing and data mining for example lint, klocwork, etc.
#
# Copyright (c) 2009-2014 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tools/build/scons/scripts/loaded_builders.py#1 $
#  $DateTime: 2016/12/13 07:57:53 $
#  $Author: mplcsds1 $
#  $Change: 11985114 $
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

def exists(env):
    '''
    exists
    '''
    return env.Detect("Loaded")

def generate(env):
    '''
    generate
    '''
    env['LOADED_BUILDER_ENV'] = env
    env['LOADED_BUILDER'] = True
    env['LOADED_BUILDERS'] = []
    env.AddMethod(add_loaded_builder, "AddLoadedBuilder")

def add_loaded_builder(env, script):
    '''
    add_loaded_builder
    '''
    usage_env = env.get('LOADED_BUILDER_ENV')
    usage_env['LOADED_BUILDERS'].append(script)

