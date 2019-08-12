# --------------------------------------------------------------------------- #
# Description   ::  Defines the SCons Builder for STM 2.0 Parser
# --------------------------------------------------------------------------- #
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tools/build/stm2_builder.py#1 $
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
#Uncomment the next line and remove below dynamic path changes at a later date
#STM_DIR = "${BUILD_ROOT}/mcs/stm/tools"

#------------------------------------------------------------------------------
#                      SCONS HOOKS (exist/generate)
#------------------------------------------------------------------------------
def exists(env):
    return env.Detect('stm2_builder')

def generate(env):
   stm_act = env.GetBuilderAction(stm_builder)
   stm_bld = env.Builder(action = stm_act, 
                             source_scanner = SCons.Scanner.C.CScanner(),
                             emitter = stm_emitter,
                             suffix = '.smh', src_suffix = '.smf')

   env.Append(BUILDERS = {'STM2' : stm_bld})


#-------------------------------------------------------------------------------
# STM2 Compiler emitter function
#
def stm_emitter(target, source, env):
    # Iterate through the list of targets looking for the SMH and SMT files
    stm_file = str(source[0])
    c_file   = str(target[0])

    env.PrintDebugInfo('STM2', "stm: " + stm_file)
    env.PrintDebugInfo('STM2', "c: " + c_file)

    # Output goes in the same location as .c file
    output_dir = os.path.dirname(c_file)
    # Output filenames are based on input .stm filename
    filebase = os.path.splitext(os.path.basename(stm_file))[0]
    doti_file   = os.path.join(output_dir, filebase + '.i')
    exth_file   = os.path.join(output_dir, '__' + filebase + '.h')
    inth_file   = os.path.join(output_dir, '__' + filebase + '_int.h')
    c_stub_file = os.path.join(output_dir, '__' + filebase + '.stub')

    # Preprocess the SMF -> SMI
    doti = env.PreProcess(doti_file, stm_file)
    env.Depends(target, doti)

    # Update the target list
    target = [env.File(c_file),
              env.File(exth_file),
              env.File(inth_file),
              env.File(c_stub_file),
              ]
    # Update the source list
    source.append(doti)
    
    return (target, source)


#-------------------------------------------------------------------------------
# STM2 Compiler builder function
#
def stm_builder(target, source, env):

    if env.PathExists('${BUILD_ROOT}/mcs/stm2/tools'):
        STM2_DIR = '${BUILD_ROOT}/mcs/stm2/tools'
    else:    
        STM2_DIR = '${BUILD_ROOT}/modem/mcs/stm2/tools'

    stm_file  = str(source[0])
    doti_file = str(source[1])

    c_file      = str(target[0])
    exth_file   = str(target[1])
    inth_file   = str(target[2])
    c_stub_file = str(target[3])

    if env['PLATFORM'] == 'linux':
        cmd = "perl " + STM2_DIR + "/stmcomp.pl"
    else:
        cmd = STM2_DIR + "/stmcomp.exe"

    # build command
    stm_cmd  = cmd
    stm_cmd += ' -oe ' + exth_file
    stm_cmd += ' -oi ' + inth_file
    stm_cmd += ' -oc ' + c_file
    stm_cmd += ' -os ' + c_stub_file
    stm_cmd += ' ' + doti_file
    
    # run command(s)
    data, err, rv = env.RunCmds([env.File(c_file), env.File(inth_file), env.File(exth_file), env.File(c_stub_file)],
                                [env.File(doti_file)],
                                stm_cmd)
