# --------------------------------------------------------------------------- #
# Description   ::  Defines the SCons Builder for STM 2.0 Parser
# --------------------------------------------------------------------------- #
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tools/build/xsd_builder.py#1 $
# --------------------------------------------------------------------------- #
# Copyright (c) 2013 Qualcomm Technologies Incorporated.
#
# All Rights Reserved. Qualcomm Confidential and Proprietary
# Export of this technology or software is regulated by the U.S. Government.
# Diversion contrary to U.S. law prohibited.
#
# All ideas, data and information contained in or disclosed by
# this document are confidential and proprietary information of
# Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
# By accepting this material the recipient agrees that this material
# and the information contained therein are held in confidence and in
# trust and will not be used, copied, reproduced in whole or in part,
# nor its contents revealed in any manner to others without the express
# written permission of Qualcomm Technologies Incorporated.
# --------------------------------------------------------------------------- #

# --------------------------------------------------------------------------- #
#                       IMPORT MODULES & DEFINITONS                           #
# --------------------------------------------------------------------------- #
import SCons.Scanner
import os

# --------------------------------------------------------------------------- #
#                       LOCAL VARIABLE DEFINITIONS                            #
# --------------------------------------------------------------------------- #

#------------------------------------------------------------------------------
#                      SCONS HOOKS (exist/generate)
#------------------------------------------------------------------------------
def exists(env):
    return env.Detect('xsd_builder')

def generate(env):
   xsd_act = env.GetBuilderAction(xsd_builder)
   xsd_bld = env.Builder(action = xsd_act, 
                         emitter = xsd_emitter,
                         suffix = '.hxx', src_suffix = '.xsd')

   env.Append(BUILDERS = {'XSD' : xsd_bld})


#-------------------------------------------------------------------------------
# STM2 Compiler emitter function
#
def xsd_emitter(target, source, env):
    # Iterate through the list of targets looking for the SMH and SMT files
    xsd_file = str(source[0])
    cpp_file = str(target[0])

    env.PrintDebugInfo('xsd', "xsd: " + xsd_file)
    env.PrintDebugInfo('xsd', "cpp: " + cpp_file)

    # Output goes in the same location as .hxx file
    output_dir = os.path.dirname(cpp_file)
    # Output filenames are based on input .xsd filename
    filebase = os.path.splitext(os.path.basename(xsd_file))[0]
    hxx_file = os.path.join(output_dir, filebase + '.hxx')

    # Update the target list
    target = [env.File(cpp_file),
              env.File(hxx_file),
              ]

    return (target, source)


#-------------------------------------------------------------------------------
# STM2 Compiler builder function
#
def xsd_builder(target, source, env):
    xsd_file  = str(source[0])

    cpp_file    = str(target[0])
    hxx_file    = str(target[1])

    output_dir = os.path.dirname(hxx_file)

    if env['PLATFORM'] == 'linux':
        pass
    else:
        cmd = "${BUILD_ROOT}/mcs/xsd/tools/xsd.exe"

    # build command
    xsd_cmd  = cmd
    xsd_cmd += ' cxx-tree'
    xsd_cmd += ' --root-element-last'
    xsd_cmd += ' --cxx-prologue "#include \\"xsd_assert.h\\""'
    xsd_cmd += ' --cxx-suffix .cpp'
    xsd_cmd += ' --output-dir ' + output_dir
    xsd_cmd += ' ' + xsd_file
    
    # run command(s)
    data, err, rv = env.RunCmds([env.File(xsd_file)],
                                [env.File(hxx_file), env.File(cpp_file)],
                                xsd_cmd)
